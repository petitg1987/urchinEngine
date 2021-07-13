#include <cmath>
#include <stdexcept>
#include <string>

#include <scene/renderer3d/lighting/shadow/ShadowManager.h>
#include <scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h>
#include <texture/filter/TextureFilter.h>
#include <texture/filter/gaussianblur/GaussianBlurFilterBuilder.h>
#include <texture/filter/downsample/DownSampleFilterBuilder.h>
#include <graphic/render/shader/builder/ShaderBuilder.h>
#include <graphic/render/target/OffscreenRender.h>

namespace urchin {

    ShadowManager::ShadowManager(LightManager& lightManager, OctreeManager<Model>& modelOctreeManager) :
            shadowMapBias(ConfigService::instance()->getFloatValue("shadow.shadowMapBias")),
            percentageUniformSplit(ConfigService::instance()->getFloatValue("shadow.frustumUniformSplitAgainstLogSplit")),
            config({}),
            lightManager(lightManager),
            modelOctreeManager(modelOctreeManager),
            bForceUpdateAllShadowMaps(false) {
        lightManager.addObserver(this, LightManager::ADD_LIGHT);
        lightManager.addObserver(this, LightManager::REMOVE_LIGHT);
    }

    void ShadowManager::setupLightingRenderer(const std::shared_ptr<GenericRendererBuilder>& lightingRendererBuilder) {
        std::size_t mLightProjectionViewSize = getMaxShadowLights() * config.nbShadowMaps;
        lightProjectionViewMatrices.resize(mLightProjectionViewSize, Matrix4<float>{});

        lightingRendererBuilder
                ->addUniformData(mLightProjectionViewSize * sizeof(Matrix4<float>), lightProjectionViewMatrices.data()) //binding 4
                ->addUniformData(config.nbShadowMaps * sizeof(float) * 4, depthSplitDistance); //binding 5
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
                    addShadowLight(light);
                }
            } else if (notificationType == LightManager::REMOVE_LIGHT) {
                light->removeObserver(this, Light::PRODUCE_SHADOW);
                if (light->isProduceShadow()) {
                    removeShadowLight(light);
                }
            }
        } else if (auto* light = dynamic_cast<Light*>(observable)) {
            if (notificationType == Light::PRODUCE_SHADOW) {
                if (light->isProduceShadow()) {
                    addShadowLight(light);
                } else {
                    removeShadowLight(light);
                }
            }
        }
    }

    unsigned int ShadowManager::getMaxShadowLights() const {
        return lightManager.getMaxLights();
    }

    float ShadowManager::getShadowMapBias() const {
        return shadowMapBias;
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

    const LightShadowMap& ShadowManager::getLightShadowMap(const Light* light) const {
        auto it = lightShadowMaps.find(light);
        if (it == lightShadowMaps.end()) {
            throw std::runtime_error("No light shadow map found for this light.");
        }

        return *it->second;
    }

    void ShadowManager::updateVisibleModels(const Frustum<float>& frustum) {
        ScopeProfiler sp(Profiler::graphic(), "upVisibleModel");

        splitFrustum(frustum);
        for (const auto& lightShadowMap : lightShadowMaps) {
            updateLightSplitsShadowMap(*lightShadowMap.second);
        }

        //store all visible models from all lights
        visibleModels.clear();
        for (const auto& lightShadowMap : lightShadowMaps) {
            for (const auto& lightSplitShadowMap : lightShadowMap.second->getLightSplitShadowMaps()) {
                OctreeableHelper<Model>::merge(visibleModels, lightSplitShadowMap->getModels());
            }
        }
    }

    /**
     * @return All visible models from all lights
     */
    const std::vector<Model*>& ShadowManager::getVisibleModels() {
        return visibleModels;
    }

    void ShadowManager::removeModel(Model* model) {
        for (auto& lightShadowMap : lightShadowMaps) {
            lightShadowMap.second->removeModel(model);
        }
    }

    void ShadowManager::addShadowLight(const Light* light) {
        auto shadowMapTexture = Texture::buildArray(config.shadowMapResolution, config.shadowMapResolution, config.nbShadowMaps, TextureFormat::RG_32_FLOAT, nullptr);
        //The shadow map must be cleared with the farthest depth value (1.0f).
        //Indeed, the shadow map is read with some imprecision and unwritten pixel could be fetched and would lead to artifact on world borders.
        shadowMapTexture->enableClearColor(Vector4<float>(1.0f, 1.0f, -1.0f, -1.0f));

        auto shadowMapRenderTarget = std::make_unique<OffscreenRender>("shadow map", RenderTarget::WRITE_ONLY_DEPTH_ATTACHMENT);
        shadowMapRenderTarget->addTexture(shadowMapTexture);

        auto newLightShadowMap = std::make_unique<LightShadowMap>(light, modelOctreeManager, config.viewingShadowDistance, shadowMapTexture, config.nbShadowMaps, std::move(shadowMapRenderTarget));
        for (unsigned int i = 0; i < config.nbShadowMaps; ++i) {
            newLightShadowMap->addLightSplitShadowMap();
        }

        //add shadow map filter
        if (config.blurShadow != BlurShadow::NO_BLUR) {
            std::unique_ptr<TextureFilter> verticalBlurFilter = std::make_unique<GaussianBlurFilterBuilder>("shadow map - vertical gaussian blur filter", shadowMapTexture)
                    ->textureSize(config.shadowMapResolution, config.shadowMapResolution)
                    ->textureType(TextureType::ARRAY)
                    ->textureNumberLayer(config.nbShadowMaps)
                    ->textureFormat(TextureFormat::RG_32_FLOAT)
                    ->blurDirection(GaussianBlurFilterBuilder::VERTICAL_BLUR)
                    ->blurSize((unsigned int)config.blurShadow)
                    ->build();

            std::unique_ptr<TextureFilter> horizontalBlurFilter = std::make_unique<GaussianBlurFilterBuilder>("shadow map - horizontal gaussian blur filter", verticalBlurFilter->getTexture())
                    ->textureSize(config.shadowMapResolution, config.shadowMapResolution)
                    ->textureType(TextureType::ARRAY)
                    ->textureNumberLayer(config.nbShadowMaps)
                    ->textureFormat(TextureFormat::RG_32_FLOAT)
                    ->blurDirection(GaussianBlurFilterBuilder::HORIZONTAL_BLUR)
                    ->blurSize((unsigned int)config.blurShadow)
                    ->build();

            newLightShadowMap->addTextureFilter(std::move(verticalBlurFilter));
            newLightShadowMap->addTextureFilter(std::move(horizontalBlurFilter));
        } else { //null filter necessary because it allows to store cached shadow map in a texture which is not cleared.
            std::unique_ptr<TextureFilter> nullFilter = std::make_unique<DownSampleFilterBuilder>("shadow map - cache null filter", shadowMapTexture)
                    ->textureSize(config.shadowMapResolution, config.shadowMapResolution)
                    ->textureType(TextureType::ARRAY)
                    ->textureNumberLayer(config.nbShadowMaps)
                    ->textureFormat(TextureFormat::RG_32_FLOAT)
                    ->build();

            newLightShadowMap->addTextureFilter(std::move(nullFilter));
        }

        lightShadowMaps[light] = std::move(newLightShadowMap);
    }

    void ShadowManager::removeShadowLight(const Light* light) {
        lightShadowMaps.erase(light);
    }

    /**
     * Updates lights data which producing shadows
     */
    void ShadowManager::updateShadowLights() {
        std::vector<const Light*> allLights;
        allLights.reserve(lightShadowMaps.size());
        for (const auto& lightShadowMap : lightShadowMaps) {
            allLights.emplace_back(lightShadowMap.first);
        }

        for (const auto& allLight : allLights) {
            removeShadowLight(allLight);
            addShadowLight(allLight);
        }
    }

    void ShadowManager::updateLightSplitsShadowMap(const LightShadowMap& lightShadowMap) {
        ScopeProfiler sp(Profiler::graphic(), "upFrustumShadow");

        if (lightShadowMap.getLight()->hasParallelBeams()) { //sun light
            unsigned int i = 0;
            for (const auto& lightSplitShadowMap : lightShadowMap.getLightSplitShadowMaps()) {
                lightSplitShadowMap->update(splitFrustums[i++], bForceUpdateAllShadowMaps);
            }
        } else {
            throw std::runtime_error("Shadow not supported on omnidirectional light.");
        }

        bForceUpdateAllShadowMaps = false;
    }

    void ShadowManager::splitFrustum(const Frustum<float>& frustum) {
        ScopeProfiler sp(Profiler::graphic(), "splitFrustum");

        splitDistances.clear();
        splitFrustums.clear();

        float nearDistance = frustum.computeNearDistance();
        float previousSplitDistance = nearDistance;
        for (unsigned int i = 1; i <= config.nbShadowMaps; ++i) {
            float uniformSplit = nearDistance + (config.viewingShadowDistance - nearDistance) * ((float)i / (float)config.nbShadowMaps);
            float logarithmicSplit = nearDistance * std::pow(config.viewingShadowDistance / nearDistance, (float)i / (float)config.nbShadowMaps);

            float splitDistance = (percentageUniformSplit * uniformSplit) + ((1.0f - percentageUniformSplit) * logarithmicSplit);

            splitDistances.push_back(splitDistance);
            splitFrustums.push_back(frustum.splitFrustum(previousSplitDistance, splitDistance));

            previousSplitDistance = splitDistance;
        }
    }

    void ShadowManager::forceUpdateAllShadowMaps() {
        bForceUpdateAllShadowMaps = true;
    }

    void ShadowManager::updateShadowMaps() {
        ScopeProfiler sp(Profiler::graphic(), "updateShadowMap");

        for (auto& lightShadowMap : lightShadowMaps) {
            lightShadowMap.second->renderModels();
            lightShadowMap.second->applyTextureFilters();
        }
    }

    void ShadowManager::loadShadowMaps(GenericRenderer& lightingRenderer, std::size_t shadowMapTexUnit) {
        std::size_t shadowLightIndex = 0;
        for (auto* visibleLight : lightManager.getVisibleLights()) {
            if (visibleLight->isProduceShadow()) {
                assert(shadowLightIndex < getMaxShadowLights());
                const auto& lightShadowMap = lightShadowMaps.find(visibleLight)->second;

                if (lightingRenderer.getUniformTextureReader(shadowMapTexUnit, shadowLightIndex)->getTexture() != lightShadowMap->getFilteredShadowMapTexture()) {
                    lightingRenderer.updateUniformTextureReaderArray(shadowMapTexUnit, shadowLightIndex, TextureReader::build(lightShadowMap->getFilteredShadowMapTexture(), TextureParam::buildLinear()));
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

        for (unsigned int shadowMapIndex = 0; shadowMapIndex < config.nbShadowMaps; ++shadowMapIndex) {
            float currSplitDistance = splitDistances[shadowMapIndex];
            depthSplitDistance[shadowMapIndex * 4] = ((projectionMatrix(2, 2) * -currSplitDistance + projectionMatrix(2, 3)) / (currSplitDistance));
        }

        lightingRenderer.updateUniformData(4, lightProjectionViewMatrices.data());
        lightingRenderer.updateUniformData(5, depthSplitDistance);
    }

}
