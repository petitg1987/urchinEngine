#include <cmath>
#include <stdexcept>
#include <string>
#include <ranges>
#include <cstring>

#include "scene/renderer3d/lighting/shadow/ShadowManager.h"
#include "scene/renderer3d/lighting/light/omnidirectional/OmnidirectionalLight.h"
#include "scene/renderer3d/lighting/light/spot/SpotLight.h"
#include "scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h"
#include "scene/renderer3d/lighting/shadow/OffsetTextureGenerator.h"

namespace urchin {

    ShadowManager::ShadowManager(const Config& config, LightManager& lightManager, ModelOcclusionCuller& modelOcclusionCuller) :
            config(config),
            lightManager(lightManager),
            modelOcclusionCuller(modelOcclusionCuller),
            splitData({}),
            shadowMapInfo({}) {
        std::memset(&shadowMapInfo, 0, sizeof(shadowMapInfo));

        checkConfig();

        lightManager.addObserver(this, LightManager::ADD_LIGHT);
        lightManager.addObserver(this, LightManager::REMOVE_LIGHT);

        emptyShadowMapTexture = Texture::buildEmptyArrayRg("empty shadow map");
        updateShadowMapOffsets();
    }

    void ShadowManager::setupDeferredSecondPassRenderer(const std::shared_ptr<GenericRendererBuilder>& deferredSecondPassRendererBuilder, uint32_t projViewMatricesUniformBinding,
                                              uint32_t shadowMapDataUniformBinding, uint32_t shadowMapInfoUniformBinding) {
        std::size_t maxMatricesLightProjectionViewSize = (std::size_t)(getMaxLightsWithShadow()) * getMaxSplitShadowMaps();
        lightProjectionViewMatrices.resize(maxMatricesLightProjectionViewSize, Matrix4<float>{});

        shadowMapInfo.offsetSampleCount = (int)(config.blurFilterBoxSize * config.blurFilterBoxSize);

        deferredSecondPassRendererBuilder
                ->addUniformData(projViewMatricesUniformBinding, maxMatricesLightProjectionViewSize * sizeof(Matrix4<float>), lightProjectionViewMatrices.data())
                ->addUniformData(shadowMapDataUniformBinding, sizeof(splitData), splitData.data())
                ->addUniformData(shadowMapInfoUniformBinding, sizeof(shadowMapInfo), &shadowMapInfo);
    }

    void ShadowManager::onCameraProjectionUpdate(const Camera&) const {
        //nothing to update
    }

    void ShadowManager::notify(Observable* observable, int notificationType) {
        if (dynamic_cast<LightManager*>(observable)) {
            Light* light = lightManager.getLastUpdatedLight();
            if (notificationType == LightManager::ADD_LIGHT) {
                light->addObserver(this, Light::PRODUCE_SHADOW);
                if (light->isProduceShadow()) {
                    addOrReplaceShadowLight(*light);
                }
            } else if (notificationType == LightManager::REMOVE_LIGHT) {
                light->removeObserver(this, Light::PRODUCE_SHADOW);
                if (light->isProduceShadow()) {
                    removeShadowLight(*light);
                }
            }
        } else if (auto* light = dynamic_cast<Light*>(observable)) {
            if (notificationType == Light::AFFECTED_ZONE_UPDATED) {
                if (lightShadowMaps.contains(light)) {
                    for (const auto& lightSplitShadowMap : getLightShadowMap(light).getLightSplitShadowMaps()) {
                        lightSplitShadowMap->onLightAffectedZoneUpdated();
                    }
                }
            } else if (notificationType == Light::PRODUCE_SHADOW) {
                if (light->isProduceShadow()) {
                    addOrReplaceShadowLight(*light);
                } else {
                    removeShadowLight(*light);
                }
            } else if (notificationType == Light::ILLUMINATED_AREA_SIZE_UPDATED) {
                if (lightShadowMaps.contains(light)) {
                    unsigned int oldSpotShadowMapResolution = getLightShadowMap(light).getShadowMapSize();
                    if (const auto* omnidirectionalLight = dynamic_cast<const OmnidirectionalLight*>(light)) {
                        unsigned int newSpotShadowMapResolution = computeShadowMapResolution(*omnidirectionalLight);
                        if (oldSpotShadowMapResolution != newSpotShadowMapResolution) {
                            addOrReplaceShadowLight(*light);
                        }
                    } else if (const auto* spotLight = dynamic_cast<const SpotLight*>(light)) {
                        unsigned int newSpotShadowMapResolution = computeShadowMapResolution(*spotLight);
                        if (oldSpotShadowMapResolution != newSpotShadowMapResolution) {
                            addOrReplaceShadowLight(*light);
                        }
                    }
                }
            }
        }
    }

    unsigned int ShadowManager::getMaxLightsWithShadow() const {
        return MAX_LIGHTS_WITH_SHADOW;
    }

    unsigned int ShadowManager::getMaxSplitShadowMaps() const {
        return std::max(std::max(1u /* spot */, 6u /* omnidirectional */), getConfig().nbSunSplitShadowMaps);
    }

    float ShadowManager::getShadowMapConstantBiasFactor() const {
        return 0.0003f;
    }

    float ShadowManager::getShadowMapSlopeBiasFactor() const {
        return 0.0007f;
    }

    unsigned int ShadowManager::getShadowMapOffsetTexSize() const {
        return SHADOW_MAP_OFFSET_TEX_SIZE;
    }

    void ShadowManager::updateConfig(const Config& config) {
        if (this->config.blurFilterBoxSize != config.blurFilterBoxSize ||
                this->config.maxLightsWithShadow != config.maxLightsWithShadow ||
                this->config.nbSunSplitShadowMaps != config.nbSunSplitShadowMaps ||
                this->config.sunShadowMapResolution != config.sunShadowMapResolution ||
                this->config.sunShadowViewDistance != config.sunShadowViewDistance ||
                this->config.omniShadowMapResolutionFactor != config.omniShadowMapResolutionFactor ||
                this->config.omniShadowMapMaxResolution != config.omniShadowMapMaxResolution ||
                this->config.spotShadowMapResolutionFactor != config.spotShadowMapResolutionFactor ||
                this->config.spotShadowMapMaxResolution != config.spotShadowMapMaxResolution) {
            bool nbSunSplitShadowMapUpdated = this->config.nbSunSplitShadowMaps != config.nbSunSplitShadowMaps;
            bool blurFilterUpdated = this->config.blurFilterBoxSize != config.blurFilterBoxSize;

            this->config = config;
            checkConfig();

            updateShadowLights();
            if (nbSunSplitShadowMapUpdated) {
                notifyObservers(this, NUMBER_SPLIT_SHADOW_MAPS_UPDATE);
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
        if (config.maxLightsWithShadow > MAX_LIGHTS_WITH_SHADOW) {
            throw std::invalid_argument("Number of lights with shadow maps must be <= " + std::to_string(MAX_LIGHTS_WITH_SHADOW) + ". Value: " + std::to_string(config.maxLightsWithShadow));
        } else if (config.nbSunSplitShadowMaps > MAX_SPLIT_SHADOW_MAPS) {
            throw std::invalid_argument("Number of sun shadow maps must be <= " + std::to_string(MAX_SPLIT_SHADOW_MAPS) + ". Value: " + std::to_string(config.nbSunSplitShadowMaps));
        } else if (config.blurFilterBoxSize == 0) {
            throw std::invalid_argument("Size of the blur filter box must be greater or equal to 1. Value: " + std::to_string(config.blurFilterBoxSize));
        }
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
        ScopeProfiler sp(Profiler::graphic(), "smUpVisModel");

        updateSplitFrustum(frustum);

        for (const std::unique_ptr<LightShadowMap>& lightShadowMap : std::views::values(lightShadowMaps)) {
            for (const auto& lightSplitShadowMap : lightShadowMap->getLightSplitShadowMaps()) {
                lightSplitShadowMap->updateVisibleModels();
            }
            lightShadowMap->updateVisibleModels();
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

    void ShadowManager::addOrReplaceShadowLight(Light& light) {
        ScopeProfiler sp(Profiler::graphic(), "addShadowLight");

        removeShadowLight(light);

        light.addObserver(this, Light::AFFECTED_ZONE_UPDATED);
        light.addObserver(this, Light::ILLUMINATED_AREA_SIZE_UPDATED);

        if (light.getLightType() == Light::LightType::SUN) {
            lightShadowMaps[&light] = std::make_unique<LightShadowMap>(false, light, modelOcclusionCuller, config.sunShadowViewDistance, config.sunShadowMapResolution, config.nbSunSplitShadowMaps);
        } else if (light.getLightType() == Light::LightType::OMNIDIRECTIONAL) {
            const auto& omnidirectionalLight = static_cast<const OmnidirectionalLight&>(light);
            unsigned int shadowMapResolution = computeShadowMapResolution(omnidirectionalLight);
            lightShadowMaps[&light] = std::make_unique<LightShadowMap>(false, light, modelOcclusionCuller, -1.0f, shadowMapResolution, 6);
        } else if (light.getLightType() == Light::LightType::SPOT) {
            const auto& spotLight = static_cast<const SpotLight&>(light);
            unsigned int shadowMapResolution = computeShadowMapResolution(spotLight);
            lightShadowMaps[&light] = std::make_unique<LightShadowMap>(false, light, modelOcclusionCuller, -1.0f, shadowMapResolution, 1);
        } else {
            throw std::runtime_error("Shadow not supported for light of type: " + std::to_string((int)light.getLightType()));
        }
    }

    unsigned int ShadowManager::computeShadowMapResolution(const OmnidirectionalLight& omnidirectionalLight) const {
        unsigned int expectedShadowMapResolution = (unsigned int)(omnidirectionalLight.computeRadius() * config.omniShadowMapResolutionFactor);
        return std::min(config.omniShadowMapMaxResolution, std::max(512u, expectedShadowMapResolution));
    }

    unsigned int ShadowManager::computeShadowMapResolution(const SpotLight& spotLight) const {
        unsigned int expectedShadowMapResolution = (unsigned int)(spotLight.computeEndRadius() * config.spotShadowMapResolutionFactor);
        return std::min(config.spotShadowMapMaxResolution, std::max(512u, expectedShadowMapResolution));
    }

    void ShadowManager::removeShadowLight(Light& light) {
        if (lightShadowMaps.erase(&light) != 0) {
            light.removeObserver(this, Light::ILLUMINATED_AREA_SIZE_UPDATED);
            light.removeObserver(this, Light::AFFECTED_ZONE_UPDATED);
        }
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
            addOrReplaceShadowLight(*light);
        }
    }

    void ShadowManager::updateSplitFrustum(const Frustum<float>& frustum) {
        splitFrustums.clear();

        constexpr float ADJUSTMENT_EXPONENT = 1.8f; //1.0 = linear distribution, >1.0 = exponential distribution
        float previousSplitDistance = 0.0f;
        for (unsigned int i = 0; i < config.nbSunSplitShadowMaps; ++i) {
            float linearSplitPerc = (((float)i) + 1.0f) / (float)config.nbSunSplitShadowMaps;
            float splitPerc = std::pow(linearSplitPerc, ADJUSTMENT_EXPONENT);
            float splitDistance = config.sunShadowViewDistance * splitPerc;

            splitFrustums.emplace_back(frustum.splitFrustum(previousSplitDistance, splitDistance));
            for (const std::unique_ptr<LightShadowMap>& lightShadowMap : std::views::values(lightShadowMaps)) {
                if (lightShadowMap->getLight().getLightType() == Light::LightType::SUN) {
                    lightShadowMap->getLightSplitShadowMaps()[i]->onSplitFrustumUpdated(splitFrustums[i]);
                }
            }

            previousSplitDistance = splitDistance;
        }
        #ifdef URCHIN_DEBUG
            assert(MathFunction::isEqual(config.sunShadowViewDistance, previousSplitDistance, 0.01f));
        #endif
    }

    //TODO update the max allowed shadow maps based on distance
    void ShadowManager::updateShadowMaps(uint32_t frameCount, unsigned int numDependenciesToShadowMaps) const {
        ScopeProfiler sp(Profiler::graphic(), "updateShadowMap");

        unsigned int renderingOrder = 0;
        for (const Light* visibleLight : lightManager.getVisibleLights()) {
            if (visibleLight->isProduceShadow()) {
                const auto& lightShadowMap = lightShadowMaps.find(visibleLight)->second;
                lightShadowMap->renderModels(frameCount, numDependenciesToShadowMaps, renderingOrder);
            }
        }
    }

    //TODO load the max allowed shadow maps based on distance
    void ShadowManager::loadShadowMaps(GenericRenderer& deferredSecondPassRenderer, uint32_t viewProjMatricesUniformBinding, uint32_t shadowMapDataUniformBinding,
                                       uint32_t shadowMapInfoUniformBinding, uint32_t texUniformBinding, uint32_t offsetTexUniformBinding) {
        //shadow map texture
        std::size_t shadowLightIndex = 0;
        for (const Light* visibleLight : lightManager.getVisibleLights()) {
            if (visibleLight->isProduceShadow()) {
                assert(shadowLightIndex < getMaxLightsWithShadow());
                const auto& lightShadowMap = lightShadowMaps.find(visibleLight)->second;

                if (deferredSecondPassRenderer.getUniformTextureReader(texUniformBinding, shadowLightIndex)->getTexture() != lightShadowMap->getShadowMapTexture().get()) {
                    //Info: anisotropy is disabled because it's causing artifact on AMD GPU
                    // Artifact: https://computergraphics.stackexchange.com/questions/5146/cascaded-shadow-maps-seams-between-cascades
                    // Solution: https://learn.microsoft.com/fr-be/windows/win32/dxtecharts/cascaded-shadow-maps?redirectedfrom=MSDN
                    TextureParam textureParam = TextureParam::build(TextureParam::ReadMode::EDGE_CLAMP, TextureParam::ReadQuality::LINEAR, TextureParam::Anisotropy::NO_ANISOTROPY);
                    deferredSecondPassRenderer.updateUniformTextureReaderArray(texUniformBinding, shadowLightIndex, TextureReader::build(lightShadowMap->getShadowMapTexture(), std::move(textureParam)));
                }

                unsigned int maxSplitShadowMaps = getMaxSplitShadowMaps();
                for (unsigned int splitShadowMapIndex = 0; splitShadowMapIndex < maxSplitShadowMaps; ++splitShadowMapIndex) {
                    std::size_t matrixIndex = shadowLightIndex * maxSplitShadowMaps + splitShadowMapIndex;
                    if (lightShadowMap->getLightSplitShadowMaps().size() > splitShadowMapIndex) {
                        const std::unique_ptr<LightSplitShadowMap>& lightSplitShadowMap = lightShadowMap->getLightSplitShadowMaps()[splitShadowMapIndex];
                        lightProjectionViewMatrices[matrixIndex] = lightSplitShadowMap->getLightProjectionViewMatrix();
                    } else {
                        lightProjectionViewMatrices[matrixIndex] = Matrix4<float>();
                    }
                }

                shadowLightIndex++;
            }
        }

        for (auto i = (unsigned int)shadowLightIndex; i < getMaxLightsWithShadow(); ++i) {
            if (deferredSecondPassRenderer.getUniformTextureReader(texUniformBinding, i)->getTexture() != getEmptyShadowMapTexture().get()) {
                //when a shadow light is removed or moved of position: reset the texture to an empty one to free the texture memory
                deferredSecondPassRenderer.updateUniformTextureReaderArray(texUniformBinding, i, TextureReader::build(getEmptyShadowMapTexture(), TextureParam::buildNearest()));
            }
        }

        //uniform data
        deferredSecondPassRenderer.updateUniformData(viewProjMatricesUniformBinding, lightProjectionViewMatrices.data());

        for (std::size_t splitShadowMapIndex = 0; splitShadowMapIndex < (std::size_t)config.nbSunSplitShadowMaps; ++splitShadowMapIndex) {
            splitData[splitShadowMapIndex] = Point4(splitFrustums[splitShadowMapIndex].getBoundingSphere().getCenterOfMass(), splitFrustums[splitShadowMapIndex].getBoundingSphere().getRadius());
        }
        deferredSecondPassRenderer.updateUniformData(shadowMapDataUniformBinding, splitData.data());

        shadowMapInfo.offsetSampleCount = (int)(config.blurFilterBoxSize * config.blurFilterBoxSize);
        deferredSecondPassRenderer.updateUniformData(shadowMapInfoUniformBinding, &shadowMapInfo);

        //shadow map offset texture
        if (deferredSecondPassRenderer.getUniformTextureReader(offsetTexUniformBinding)->getTexture() != shadowMapOffsetTexture.get()) {
            deferredSecondPassRenderer.updateUniformTextureReader(offsetTexUniformBinding, TextureReader::build(shadowMapOffsetTexture, TextureParam::buildNearest()));
        }
    }

}
