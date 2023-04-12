#include <cmath>
#include <stdexcept>
#include <string>

#include <scene/renderer3d/lighting/shadow/ShadowManager.h>
#include <scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h>
#include <texture/filter/TextureFilter.h>
#include <texture/filter/gaussianblur/GaussianBlurFilterBuilder.h>

namespace urchin {

    ShadowManager::ShadowManager(const Config& config, LightManager& lightManager, ModelOcclusionCuller& modelOcclusionCuller) :
            config(config),
            lightManager(lightManager),
            modelOcclusionCuller(modelOcclusionCuller),
            depthSplitDistance({}) {
        lightManager.addObserver(this, LightManager::ADD_LIGHT);
        lightManager.addObserver(this, LightManager::REMOVE_LIGHT);

        emptyShadowMapTexture = Texture::buildEmptyArrayRg("empty shadow map");
    }

    void ShadowManager::setupLightingRenderer(const std::shared_ptr<GenericRendererBuilder>& lightingRendererBuilder) {
        std::size_t mLightProjectionViewSize = getMaxShadowLights() * config.nbShadowMaps;
        lightProjectionViewMatrices.resize(mLightProjectionViewSize, Matrix4<float>{});

        lightingRendererBuilder
                ->addUniformData(mLightProjectionViewSize * sizeof(Matrix4<float>), lightProjectionViewMatrices.data())
                ->addUniformData(config.nbShadowMaps * sizeof(float) * 4, depthSplitDistance.data());
    }

    void ShadowManager::onCameraProjectionUpdate(const Camera& camera) {
        this->projectionMatrix = camera.getProjectionMatrix();

        splitFrustum(camera.getFrustum());
        updateShadowLights();
    }

    void ShadowManager::notify(Observable* observable, int notificationType) {
        if (dynamic_cast<LightManager*>(observable)) {
            Light* light = lightManager.getLastUpdatedLight();
            if (notificationType == LightManager::ADD_LIGHT) {
                light->addObserver(this, Light::PRODUCE_SHADOW);
                if (light->isProduceShadow()) {
                    addShadowLight(*light);
                }
            } else if (notificationType == LightManager::REMOVE_LIGHT) {
                light->removeObserver(this, Light::PRODUCE_SHADOW);
                if (light->isProduceShadow()) {
                    removeShadowLight(*light);
                }
            }
        } else if (auto* light = dynamic_cast<Light*>(observable)) {
            if (notificationType == Light::PRODUCE_SHADOW) {
                if (light->isProduceShadow()) {
                    addShadowLight(*light);
                } else {
                    removeShadowLight(*light);
                }
            }
        }
    }

    unsigned int ShadowManager::getMaxShadowLights() const {
        return lightManager.getMaxLights();
    }

    float ShadowManager::getShadowMapConstantBias() const {
        return SHADOW_MAP_CONSTANT_BIAS;
    }

    float ShadowManager::getShadowMapSlopeBiasFactor() const {
        return SHADOW_MAP_SLOPE_BIAS_FACTOR;
    }

    void ShadowManager::updateConfig(const Config& config) {
        if (this->config.nbShadowMaps != config.nbShadowMaps ||
                this->config.shadowMapResolution != config.shadowMapResolution ||
                this->config.viewingShadowDistance != config.viewingShadowDistance ||
                this->config.blurShadow != config.blurShadow) {
            bool nbShadowMapUpdated = this->config.nbShadowMaps != config.nbShadowMaps;

            this->config = config;
            checkConfig();

            updateShadowLights();
            if (nbShadowMapUpdated) {
                notifyObservers(this, ShadowManager::NUMBER_SHADOW_MAPS_UPDATE);
            }
        }
    }

    const ShadowManager::Config& ShadowManager::getConfig() const {
        return config;
    }

    void ShadowManager::checkConfig() const {
        if (config.nbShadowMaps <= 1) { //note: shadow maps texture array with depth = 1 generate error in GLSL texture() function
            throw std::invalid_argument("Number of shadow maps must be greater than one. Value: " + std::to_string(config.nbShadowMaps));
        } else if (config.nbShadowMaps > SHADOW_MAPS_SHADER_LIMIT) {
            throw std::invalid_argument("Number of shadow maps must be lower than " + std::to_string(SHADOW_MAPS_SHADER_LIMIT) + ". Value: " + std::to_string(config.nbShadowMaps));
        }
    }

    const std::vector<Frustum<float>>& ShadowManager::getSplitFrustums() const {
        return splitFrustums;
    }

    const std::shared_ptr<Texture>& ShadowManager::getEmptyShadowMapTexture() const {
        return emptyShadowMapTexture;
    }

    const LightShadowMap& ShadowManager::getLightShadowMap(const Light* light) const {
        auto itFind = lightShadowMaps.find(const_cast<Light*>(light));
        if (itFind == lightShadowMaps.end()) {
            throw std::runtime_error("No light shadow map found for this light.");
        }

        return *itFind->second;
    }

    void ShadowManager::updateVisibleModels(const Frustum<float>& frustum) {
        ScopeProfiler sp(Profiler::graphic(), "upVisibleModel");

        splitFrustum(frustum);
        for (const auto& [light, lightShadowMap] : lightShadowMaps) {
            updateLightSplitsShadowMap(*lightShadowMap);
        }

        //store all visible models from all lights
        visibleModels.clear();
        for (const auto& [light, lightShadowMap] : lightShadowMaps) {
            for (const auto& lightSplitShadowMap : lightShadowMap->getLightSplitShadowMaps()) {
                OctreeableHelper<Model>::merge(visibleModels, lightSplitShadowMap->getModels());
            }
        }
    }

    /**
     * @return All visible models from all lights
     */
    std::span<Model* const> ShadowManager::getVisibleModels() const {
        return visibleModels;
    }

    void ShadowManager::removeModel(Model* model) const {
        for (const auto& [light, lightShadowMap] : lightShadowMaps) {
            lightShadowMap->removeModel(model);
        }
    }

    void ShadowManager::addShadowLight(Light& light) {
        ScopeProfiler sp(Profiler::graphic(), "addShadowLight");

        auto shadowMapRenderTarget = std::make_unique<OffscreenRender>("shadow map", RenderTarget::LOCAL_DEPTH_ATTACHMENT);
        auto shadowMapTexture = Texture::buildArray("shadow map", config.shadowMapResolution, config.shadowMapResolution, config.nbShadowMaps, TextureFormat::RG_32_FLOAT);
        //The shadow map must be cleared with the farthest depth value (1.0f).
        //Indeed, the shadow map is read with some imprecision and unwritten pixel could be fetched and would lead to artifact on world borders.
        Vector4 clearShadowMapColor(1.0f, 1.0f, -1.0f, -1.0f);
        shadowMapRenderTarget->addOutputTexture(shadowMapTexture, LoadType::LOAD_CLEAR, std::make_optional(clearShadowMapColor));

        auto newLightShadowMap = std::make_unique<LightShadowMap>(light, modelOcclusionCuller, config.viewingShadowDistance, shadowMapTexture, config.nbShadowMaps, std::move(shadowMapRenderTarget));
        for (unsigned int i = 0; i < config.nbShadowMaps; ++i) {
            newLightShadowMap->addLightSplitShadowMap();
        }

        //add shadow map filter
        if (config.blurShadow != BlurShadow::NO_BLUR) {
            std::unique_ptr<TextureFilter> verticalBlurFilter = std::make_unique<GaussianBlurFilterBuilder>(false, "shadow map - vertical gaussian blur filter", shadowMapTexture)
                    ->textureSize(config.shadowMapResolution, config.shadowMapResolution)
                    ->textureType(TextureType::ARRAY)
                    ->textureNumberLayer(config.nbShadowMaps)
                    ->textureFormat(TextureFormat::RG_32_FLOAT)
                    ->blurDirection(GaussianBlurFilterBuilder::VERTICAL_BLUR)
                    ->blurSize((unsigned int)config.blurShadow)
                    ->build();

            std::unique_ptr<TextureFilter> horizontalBlurFilter = std::make_unique<GaussianBlurFilterBuilder>(false, "shadow map - horizontal gaussian blur filter", verticalBlurFilter->getTexture())
                    ->textureSize(config.shadowMapResolution, config.shadowMapResolution)
                    ->textureType(TextureType::ARRAY)
                    ->textureNumberLayer(config.nbShadowMaps)
                    ->textureFormat(TextureFormat::RG_32_FLOAT)
                    ->blurDirection(GaussianBlurFilterBuilder::HORIZONTAL_BLUR)
                    ->blurSize((unsigned int)config.blurShadow)
                    ->build();

            newLightShadowMap->addTextureFilter(std::move(verticalBlurFilter));
            newLightShadowMap->addTextureFilter(std::move(horizontalBlurFilter));
        }

        lightShadowMaps[&light] = std::move(newLightShadowMap);
    }

    void ShadowManager::removeShadowLight(Light& light) {
        lightShadowMaps.erase(&light);
    }

    /**
     * Updates lights data which producing shadows
     */
    void ShadowManager::updateShadowLights() {
        std::vector<Light*> allLights;
        allLights.reserve(lightShadowMaps.size());
        for (const auto& [light, lightShadowMap] : lightShadowMaps) {
            allLights.emplace_back(light);
        }

        for (Light* light : allLights) {
            removeShadowLight(*light);
            addShadowLight(*light);
        }
    }

    void ShadowManager::updateLightSplitsShadowMap(const LightShadowMap& lightShadowMap) {
        ScopeProfiler sp(Profiler::graphic(), "upFrustumShadow");

        if (lightShadowMap.getLight().getLightType() == Light::LightType::SUN) {
            unsigned int i = 0;
            for (const auto& lightSplitShadowMap : lightShadowMap.getLightSplitShadowMaps()) {
                lightSplitShadowMap->update(splitFrustums[i++]);
            }
        } else {
            throw std::runtime_error("Shadow not supported on omnidirectional light.");
        }
    }

    void ShadowManager::splitFrustum(const Frustum<float>& frustum) {
        splitDistances.clear();
        splitFrustums.clear();

        float sliceLength = config.viewingShadowDistance / (float)(pow(2, config.nbShadowMaps) - 1);
        float previousSplitDistance = 0.0f;
        for (unsigned int i = 1; i <= config.nbShadowMaps; ++i) {
            auto nbSlices = (float)(pow(2, i) - 1);
            float splitDistance = sliceLength * nbSlices;

            splitDistances.push_back(splitDistance);
            splitFrustums.push_back(frustum.splitFrustum(previousSplitDistance, splitDistance));
            previousSplitDistance = splitDistance;
        }
        #ifdef URCHIN_DEBUG
            assert(MathFunction::isEqual(config.viewingShadowDistance, previousSplitDistance, 0.01f));
        #endif
    }

    void ShadowManager::updateShadowMaps(std::uint32_t frameIndex, unsigned int numDependenciesToShadowMaps) const {
        ScopeProfiler sp(Profiler::graphic(), "updateShadowMap");
        unsigned int renderingOrder = 0;

        for (const auto& [light, lightShadowMap] : lightShadowMaps) {
            if (lightShadowMap->hasTextureFilter()) {
                unsigned int numDependenciesToRawShadowMaps = 1 /* first texture filter */;
                lightShadowMap->renderModels(frameIndex, numDependenciesToRawShadowMaps, renderingOrder);
                lightShadowMap->applyTextureFilters(frameIndex, numDependenciesToShadowMaps);
            } else {
                unsigned int numDependenciesToRawShadowMaps = numDependenciesToShadowMaps;
                lightShadowMap->renderModels(frameIndex, numDependenciesToRawShadowMaps, renderingOrder);
            }
        }
    }

    void ShadowManager::loadShadowMaps(GenericRenderer& lightingRenderer, std::size_t shadowMapTexUnit) {
        std::size_t shadowLightIndex = 0;
        for (const Light* visibleLight : lightManager.getVisibleLights()) {
            if (visibleLight->isProduceShadow()) {
                assert(shadowLightIndex < getMaxShadowLights());
                const auto& lightShadowMap = lightShadowMaps.find(const_cast<Light*>(visibleLight))->second;

                if (lightingRenderer.getUniformTextureReader(shadowMapTexUnit, shadowLightIndex)->getTexture() != lightShadowMap->getFilteredShadowMapTexture().get()) {
                    //Info: anisotropy is disabled because it's causing artifact on AMD GPU
                    // Artifact: https://computergraphics.stackexchange.com/questions/5146/cascaded-shadow-maps-seams-between-cascades
                    // Solution: https://learn.microsoft.com/fr-be/windows/win32/dxtecharts/cascaded-shadow-maps?redirectedfrom=MSDN
                    TextureParam textureParam = TextureParam::build(TextureParam::ReadMode::EDGE_CLAMP, TextureParam::ReadQuality::LINEAR, TextureParam::Anisotropy::NO_ANISOTROPY);
                    lightingRenderer.updateUniformTextureReaderArray(shadowMapTexUnit, shadowLightIndex, TextureReader::build(lightShadowMap->getFilteredShadowMapTexture(), std::move(textureParam)));
                }

                unsigned int shadowMapIndex = 0;
                for (const auto& lightSplitShadowMap : lightShadowMap->getLightSplitShadowMaps()) {
                    std::size_t matrixIndex = shadowLightIndex * getMaxShadowLights() + shadowMapIndex;
                    lightProjectionViewMatrices[matrixIndex] = lightSplitShadowMap->getLightProjectionMatrix() * lightShadowMap->getLightViewMatrix();
                    shadowMapIndex++;
                }

                shadowLightIndex++;
            }
        }

        for (auto i = (unsigned int)shadowLightIndex; i < getMaxShadowLights(); ++i) {
            if (lightingRenderer.getUniformTextureReader(shadowMapTexUnit, i)->getTexture() != getEmptyShadowMapTexture().get()) {
                //when a shadow light is removed or moved of position: reset the texture to an empty one to free the texture memory
                lightingRenderer.updateUniformTextureReaderArray(shadowMapTexUnit, i, TextureReader::build(getEmptyShadowMapTexture(), TextureParam::buildNearest()));
            }
        }

        for (std::size_t shadowMapIndex = 0; shadowMapIndex < (std::size_t)config.nbShadowMaps; ++shadowMapIndex) {
            float currSplitDistance = splitDistances[shadowMapIndex];
            depthSplitDistance[shadowMapIndex * 4] = (projectionMatrix(2, 2) * -currSplitDistance + projectionMatrix(2, 3)) / currSplitDistance;
        }

        lightingRenderer.updateUniformData(3, lightProjectionViewMatrices.data());
        lightingRenderer.updateUniformData(4, depthSplitDistance.data());
    }

}
