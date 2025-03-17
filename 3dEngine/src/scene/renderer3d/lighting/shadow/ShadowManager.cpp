#include <cmath>
#include <stdexcept>
#include <string>
#include <ranges>
#include <cstring>

#include <scene/renderer3d/lighting/shadow/ShadowManager.h>
#include <scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h>
#include <scene/renderer3d/lighting/shadow/OffsetTextureGenerator.h>

namespace urchin {

    ShadowManager::ShadowManager(const Config& config, LightManager& lightManager, ModelOcclusionCuller& modelOcclusionCuller) :
            config(config),
            lightManager(lightManager),
            modelOcclusionCuller(modelOcclusionCuller),
            splitData({}),
            shadowMapInfo({}) {
        std::memset(&shadowMapInfo, 0, sizeof(shadowMapInfo));

        lightManager.addObserver(this, LightManager::ADD_LIGHT);
        lightManager.addObserver(this, LightManager::REMOVE_LIGHT);

        emptyShadowMapTexture = Texture::buildEmptyArrayRg("empty shadow map");
        updateShadowMapOffsets();
    }

    void ShadowManager::setupDeferredSecondPassRenderer(const std::shared_ptr<GenericRendererBuilder>& deferredSecondPassRendererBuilder, uint32_t projViewMatricesUniformBinding,
                                              uint32_t shadowMapDataUniformBinding, uint32_t shadowMapInfoUniformBinding) {
        std::size_t mLightProjectionViewSize = (std::size_t)(getMaxShadowLights()) * config.nbShadowMaps;
        lightProjectionViewMatrices.resize(mLightProjectionViewSize, Matrix4<float>{});

        shadowMapInfo.shadowMapInvSize = 1.0f / (float)config.shadowMapResolution;
        shadowMapInfo.offsetSampleCount = (int)(config.blurFilterBoxSize * config.blurFilterBoxSize);
        shadowMapInfo.shadowStrengthFactor = config.shadowStrengthFactor;

        deferredSecondPassRendererBuilder
                ->addUniformData(projViewMatricesUniformBinding, mLightProjectionViewSize * sizeof(Matrix4<float>), lightProjectionViewMatrices.data())
                ->addUniformData(shadowMapDataUniformBinding, config.nbShadowMaps * sizeof(Point4<float>), splitData.data())
                ->addUniformData(shadowMapInfoUniformBinding, sizeof(shadowMapInfo), &shadowMapInfo);
    }

    void ShadowManager::onCameraProjectionUpdate(const Camera& camera) {
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

    unsigned int ShadowManager::getShadowMapOffsetTexSize() const {
        return SHADOW_MAP_OFFSET_TEX_SIZE;
    }

    void ShadowManager::updateConfig(const Config& config) {
        if (this->config.nbShadowMaps != config.nbShadowMaps ||
                this->config.shadowMapResolution != config.shadowMapResolution ||
                this->config.viewingShadowDistance != config.viewingShadowDistance ||
                this->config.blurFilterBoxSize != config.blurFilterBoxSize) {
            bool nbShadowMapUpdated = this->config.nbShadowMaps != config.nbShadowMaps;
            bool blurFilterUpdated = this->config.blurFilterBoxSize != config.blurFilterBoxSize;

            this->config = config;
            checkConfig();

            updateShadowLights();
            if (nbShadowMapUpdated) {
                notifyObservers(this, NUMBER_SHADOW_MAPS_UPDATE);
            }
            if (blurFilterUpdated) {
                updateShadowMapOffsets();
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
        } else if (config.blurFilterBoxSize == 0) {
            throw std::invalid_argument("Size of the blur filter box must be greater or equal to 1. Value: " + std::to_string(config.blurFilterBoxSize));
        }
    }

    const std::vector<SplitFrustum>& ShadowManager::getSplitFrustums() const {
        return splitFrustums;
    }

    const std::shared_ptr<Texture>& ShadowManager::getEmptyShadowMapTexture() const {
        return emptyShadowMapTexture;
    }

    const LightShadowMap& ShadowManager::getLightShadowMap(const Light* light) const {
        auto itFind = lightShadowMaps.find(light);
        if (itFind == lightShadowMaps.end()) {
            throw std::runtime_error("No light shadow map found for this light.");
        }

        return *itFind->second;
    }

    void ShadowManager::updateVisibleModels(const Frustum<float>& frustum) {
        ScopeProfiler sp(Profiler::graphic(), "upVisibleModel");

        splitFrustum(frustum);
        for (const std::unique_ptr<LightShadowMap>& lightShadowMap : std::views::values(lightShadowMaps)) {
            updateLightSplitsShadowMap(*lightShadowMap);
        }
    }

    void ShadowManager::removeModel(Model* model) const {
        for (const std::unique_ptr<LightShadowMap>& lightShadowMap : std::views::values(lightShadowMaps)) {
            lightShadowMap->removeModel(model);
        }
    }

    void ShadowManager::updateShadowMapOffsets() {
        shadowMapOffsetTexture = OffsetTextureGenerator(getShadowMapOffsetTexSize(), config.blurFilterBoxSize).getOffsetTexture();
    }

    void ShadowManager::addShadowLight(Light& light) {
        ScopeProfiler sp(Profiler::graphic(), "addShadowLight");

        if (light.getLightType() == Light::LightType::SUN) {
            lightShadowMaps[&light] = std::make_unique<LightShadowMap>(false, light, modelOcclusionCuller, config.viewingShadowDistance, config.shadowMapResolution, config.nbShadowMaps);
        } else if (light.getLightType() == Light::LightType::SPOT) {
            //TODO review resolution for spot
            lightShadowMaps[&light] = std::make_unique<LightShadowMap>(false, light, modelOcclusionCuller, -1.0f, 1024, 1);
        } else {
            throw std::runtime_error("Shadow currently not supported for light of type: " + std::to_string((int)light.getLightType()));
        }
    }

    void ShadowManager::removeShadowLight(const Light& light) {
        lightShadowMaps.erase(&light);
    }

    /**
     * Updates lights data which producing shadows
     */
    void ShadowManager::updateShadowLights() {
        std::vector<Light*> allLights;
        allLights.reserve(lightShadowMaps.size());
        for (const std::unique_ptr<LightShadowMap>& lightShadowMap : std::views::values(lightShadowMaps)) {
            allLights.emplace_back(&lightShadowMap->getLight());
        }

        for (Light* light : allLights) {
            removeShadowLight(*light);
            addShadowLight(*light);
        }
    }

    void ShadowManager::updateLightSplitsShadowMap(const LightShadowMap& lightShadowMap) const {
        ScopeProfiler sp(Profiler::graphic(), "upFrustumShadow");

        unsigned int i = 0;
        for (const auto& lightSplitShadowMap : lightShadowMap.getLightSplitShadowMaps()) {
            lightSplitShadowMap->update(splitFrustums[i++]);
        }
    }

    void ShadowManager::splitFrustum(const Frustum<float>& frustum) {
        splitFrustums.clear();

        constexpr float ADJUSTMENT_EXPONENT = 1.4f; //1.0 = linear distribution, >1.0 = exponential distribution
        float previousSplitDistance = 0.0f;
        for (unsigned int i = 1; i <= config.nbShadowMaps; ++i) {
            float linearSplitPerc = (float)i / (float)config.nbShadowMaps;
            float splitPerc = std::pow(linearSplitPerc, ADJUSTMENT_EXPONENT);
            float splitDistance = config.viewingShadowDistance * splitPerc;

            splitFrustums.emplace_back(frustum.splitFrustum(previousSplitDistance, splitDistance));
            previousSplitDistance = splitDistance;
        }
        #ifdef URCHIN_DEBUG
            assert(MathFunction::isEqual(config.viewingShadowDistance, previousSplitDistance, 0.01f));
        #endif
    }

    void ShadowManager::updateShadowMaps(uint32_t frameIndex, unsigned int numDependenciesToShadowMaps) const {
        ScopeProfiler sp(Profiler::graphic(), "updateShadowMap");

        unsigned int renderingOrder = 0;
        for (const std::unique_ptr<LightShadowMap>& lightShadowMap : std::views::values(lightShadowMaps)) {
            lightShadowMap->renderModels(frameIndex, numDependenciesToShadowMaps, renderingOrder);
        }
    }

    void ShadowManager::loadShadowMaps(GenericRenderer& deferredSecondPassRenderer, uint32_t viewProjMatricesUniformBinding, uint32_t shadowMapDataUniformBinding,
                                       uint32_t shadowMapInfoUniformBinding, uint32_t texUniformBinding, uint32_t offsetTexUniformBinding) {
        //shadow map texture
        std::size_t shadowLightIndex = 0;
        for (const Light* visibleLight : lightManager.getVisibleLights()) {
            if (visibleLight->isProduceShadow()) {
                assert(shadowLightIndex < getMaxShadowLights());
                const auto& lightShadowMap = lightShadowMaps.find(visibleLight)->second;

                if (deferredSecondPassRenderer.getUniformTextureReader(texUniformBinding, shadowLightIndex)->getTexture() != lightShadowMap->getShadowMapTexture().get()) {
                    //Info: anisotropy is disabled because it's causing artifact on AMD GPU
                    // Artifact: https://computergraphics.stackexchange.com/questions/5146/cascaded-shadow-maps-seams-between-cascades
                    // Solution: https://learn.microsoft.com/fr-be/windows/win32/dxtecharts/cascaded-shadow-maps?redirectedfrom=MSDN
                    TextureParam textureParam = TextureParam::build(TextureParam::ReadMode::EDGE_CLAMP, TextureParam::ReadQuality::LINEAR, TextureParam::Anisotropy::NO_ANISOTROPY);
                    deferredSecondPassRenderer.updateUniformTextureReaderArray(texUniformBinding, shadowLightIndex, TextureReader::build(lightShadowMap->getShadowMapTexture(), std::move(textureParam)));
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
            if (deferredSecondPassRenderer.getUniformTextureReader(texUniformBinding, i)->getTexture() != getEmptyShadowMapTexture().get()) {
                //when a shadow light is removed or moved of position: reset the texture to an empty one to free the texture memory
                deferredSecondPassRenderer.updateUniformTextureReaderArray(texUniformBinding, i, TextureReader::build(getEmptyShadowMapTexture(), TextureParam::buildNearest()));
            }
        }

        //uniform data
        deferredSecondPassRenderer.updateUniformData(viewProjMatricesUniformBinding, lightProjectionViewMatrices.data());

        for (std::size_t shadowMapIndex = 0; shadowMapIndex < (std::size_t)config.nbShadowMaps; ++shadowMapIndex) {
            splitData[shadowMapIndex] = Point4(splitFrustums[shadowMapIndex].getBoundingSphere().getCenterOfMass(), splitFrustums[shadowMapIndex].getBoundingSphere().getRadius());
        }
        deferredSecondPassRenderer.updateUniformData(shadowMapDataUniformBinding, splitData.data());

        shadowMapInfo.shadowMapInvSize = 1.0f / (float)config.shadowMapResolution;
        shadowMapInfo.offsetSampleCount = (int)(config.blurFilterBoxSize * config.blurFilterBoxSize);
        shadowMapInfo.shadowStrengthFactor = config.shadowStrengthFactor;
        deferredSecondPassRenderer.updateUniformData(shadowMapInfoUniformBinding, &shadowMapInfo);

        //shadow map offset texture
        if (deferredSecondPassRenderer.getUniformTextureReader(offsetTexUniformBinding)->getTexture() != shadowMapOffsetTexture.get()) {
            deferredSecondPassRenderer.updateUniformTextureReader(offsetTexUniformBinding, TextureReader::build(shadowMapOffsetTexture, TextureParam::buildNearest()));
        }
    }

}
