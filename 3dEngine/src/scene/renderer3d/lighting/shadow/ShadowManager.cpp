#include <cmath>
#include <sstream>
#include <stdexcept>
#include <string>

#include "ShadowManager.h"
#include "scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h"
#include "texture/filter/TextureFilter.h"
#include "texture/filter/gaussianblur/GaussianBlurFilterBuilder.h"
#include "texture/filter/downsample/DownSampleFilterBuilder.h"
#include "graphic/shader/builder/ShaderBuilder.h"
#include "graphic/shader/data/ShaderDataSender.h"
#include "graphic/render/target/OffscreenRender.h"

#define DEFAULT_NUMBER_SHADOW_MAPS 5
#define DEFAULT_SHADOW_MAP_RESOLUTION 1024
#define DEFAULT_VIEWING_SHADOW_DISTANCE 75.0
#define DEFAULT_BLUR_SHADOW BlurShadow::MEDIUM

namespace urchin {

    ShadowManager::ShadowManager(LightManager* lightManager, OctreeManager<Model>* modelOctreeManager) :
            shadowMapBias(ConfigService::instance()->getFloatValue("shadow.shadowMapBias")),
            percentageUniformSplit(ConfigService::instance()->getFloatValue("shadow.frustumUniformSplitAgainstLogSplit")),
            shadowMapResolution(DEFAULT_SHADOW_MAP_RESOLUTION),
            nbShadowMaps(DEFAULT_NUMBER_SHADOW_MAPS),
            viewingShadowDistance(DEFAULT_VIEWING_SHADOW_DISTANCE),
            blurShadow(DEFAULT_BLUR_SHADOW),
            lightManager(lightManager),
            modelOctreeManager(modelOctreeManager),
            bForceUpdateAllShadowMaps(false),
            mLightProjectionViewShaderVar(nullptr),
            lightProjectionViewMatrices(nullptr) {
        lightManager->addObserver(this, LightManager::ADD_LIGHT);
        lightManager->addObserver(this, LightManager::REMOVE_LIGHT);
    }

    ShadowManager::~ShadowManager() {
        for (auto& lightShadowMap : lightShadowMaps) {
            delete lightShadowMap.second;
        }

        deleteLightsLocation();
        delete[] lightProjectionViewMatrices;
    }

    void ShadowManager::initiateShaderVariables(const std::shared_ptr<Shader>& lightingShader) {
        //light information
        deleteLightsLocation();
        mLightProjectionViewShaderVar = new ShaderVar*[getMaxShadowLights()];
        std::ostringstream shadowMapTextureLocName, mLightProjectionViewLocName;
        for (unsigned int i = 0; i < getMaxShadowLights(); ++i) {
            //light projection matrices
            mLightProjectionViewShaderVar[i] = new ShaderVar[nbShadowMaps];
            for (unsigned int j = 0; j < nbShadowMaps; ++j) {
                mLightProjectionViewLocName.str("");
                mLightProjectionViewLocName << "mLightProjectionView[" << i << "][" << j << "]";
                mLightProjectionViewShaderVar[i][j] = ShaderVar(lightingShader, mLightProjectionViewLocName.str());
            }
        }

        //shadow information
        depthSplitDistanceShaderVar = ShaderVar(lightingShader, "depthSplitDistance");
    }

    void ShadowManager::setupLightingRenderer(const std::unique_ptr<GenericRendererBuilder>& lightingRendererBuilder) {
        std::size_t mLightProjectionViewSize = getMaxShadowLights() * nbShadowMaps;
        lightProjectionViewMatrices = new Matrix4<float>[mLightProjectionViewSize];

        //Vulkan source code:
        lightingRendererBuilder
                //Vulkan source code: ->addShaderData(mLightProjectionViewSize * sizeof(Matrix4<float>), lightProjectionViewMatrices) //binding 4
                ->addShaderData(ShaderDataSender(true).sendData(depthSplitDistanceShaderVar, nbShadowMaps, depthSplitDistance)); //binding 5
    }

    void ShadowManager::deleteLightsLocation() {
        if (mLightProjectionViewShaderVar) {
            for (unsigned int i = 0; i < getMaxShadowLights(); ++i) {
                delete[] mLightProjectionViewShaderVar[i];
            }
        }
    }

    void ShadowManager::onCameraProjectionUpdate(const Camera* camera) {
        this->projectionMatrix = camera->getProjectionMatrix();

        splitFrustum(camera->getFrustum());
        updateShadowLights();
    }

    void ShadowManager::notify(Observable* observable, int notificationType) {
        if (dynamic_cast<LightManager*>(observable)) {
            Light* light = lightManager->getLastUpdatedLight();
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
        return lightManager->getMaxLights();
    }

    float ShadowManager::getShadowMapBias() const {
        return shadowMapBias;
    }

    void ShadowManager::setShadowMapResolution(unsigned int shadowMapResolution) {
        this->shadowMapResolution = shadowMapResolution;

        updateShadowLights();
    }

    unsigned int ShadowManager::getShadowMapResolution() const {
        return shadowMapResolution;
    }

    void ShadowManager::setNumberShadowMaps(unsigned int nbShadowMaps) {
        if (nbShadowMaps <= 1) { //note: shadow maps texture array with depth = 1 generate error in GLSL texture2DArray function
            throw std::runtime_error("Number of shadow maps must be greater than one. Value: " + std::to_string(nbShadowMaps));
        } else if (nbShadowMaps > MAX_NB_SHADOW_MAPS) {
            throw std::runtime_error("Number of shadow maps must be lower than " + std::to_string(MAX_NB_SHADOW_MAPS) + ". Value: " + std::to_string(nbShadowMaps));
        }

        this->nbShadowMaps = nbShadowMaps;

        updateShadowLights();
        notifyObservers(this, ShadowManager::NUMBER_SHADOW_MAPS_UPDATE);
    }

    unsigned int ShadowManager::getNumberShadowMaps() const {
        return nbShadowMaps;
    }

    void ShadowManager::setViewingShadowDistance(float viewingShadowDistance) {
        this->viewingShadowDistance = viewingShadowDistance;

        updateShadowLights();
    }

    float ShadowManager::getViewingShadowDistance() const {
        return viewingShadowDistance;
    }

    void ShadowManager::setBlurShadow(ShadowManager::BlurShadow blurShadow) {
        this->blurShadow = blurShadow;
        updateShadowLights();
    }

    ShadowManager::BlurShadow ShadowManager::getBlurShadow() const {
        return blurShadow;
    }

    const std::vector<Frustum<float>>& ShadowManager::getSplitFrustums() const {
        return splitFrustums;
    }

    const LightShadowMap& ShadowManager::getLightShadowMap(const Light* light) const {
        auto it = lightShadowMaps.find(light);
        if (it == lightShadowMaps.end()) {
            throw std::runtime_error("No light shadow map found for this light.");
        }

        const LightShadowMap* lightShadowMap = it->second;
        return *lightShadowMap;
    }

    void ShadowManager::updateVisibleModels(const Frustum<float>& frustum) {
        ScopeProfiler sp(Profiler::graphic(), "upVisibleModel");

        splitFrustum(frustum);
        for (auto& lightShadowMap : lightShadowMaps) {
            updateLightSplitsShadowMap(lightShadowMap.second);
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
        auto depthTexture = Texture::buildArray(shadowMapResolution, shadowMapResolution, nbShadowMaps, TextureFormat::DEPTH_32_FLOAT, nullptr);
        auto shadowMapTexture = Texture::buildArray(shadowMapResolution, shadowMapResolution, nbShadowMaps, TextureFormat::RG_32_FLOAT, nullptr);

        auto shadowMapRenderTarget = std::make_unique<OffscreenRender>();
        shadowMapRenderTarget->onResize(shadowMapResolution, shadowMapResolution);
        shadowMapRenderTarget->addTexture(depthTexture);
        shadowMapRenderTarget->addTexture(shadowMapTexture);

        auto* newLightShadowMap = new LightShadowMap(light, modelOctreeManager, viewingShadowDistance, shadowMapTexture, nbShadowMaps, std::move(shadowMapRenderTarget));
        for (unsigned int i = 0; i < nbShadowMaps; ++i) {
            newLightShadowMap->addLightSplitShadowMap();
        }

        //add shadow map filter
        if (blurShadow != BlurShadow::NO_BLUR) {
            std::unique_ptr<TextureFilter> verticalBlurFilter = std::make_unique<GaussianBlurFilterBuilder>(shadowMapTexture)
                    ->textureSize(shadowMapResolution, shadowMapResolution)
                    ->textureType(TextureType::ARRAY)
                    ->textureNumberLayer(nbShadowMaps)
                    ->textureFormat(TextureFormat::RG_32_FLOAT)
                    ->blurDirection(GaussianBlurFilterBuilder::VERTICAL_BLUR)
                    ->blurSize((unsigned int)blurShadow)
                    ->build();

            std::unique_ptr<TextureFilter> horizontalBlurFilter = std::make_unique<GaussianBlurFilterBuilder>(verticalBlurFilter->getTexture())
                    ->textureSize(shadowMapResolution, shadowMapResolution)
                    ->textureType(TextureType::ARRAY)
                    ->textureNumberLayer(nbShadowMaps)
                    ->textureFormat(TextureFormat::RG_32_FLOAT)
                    ->blurDirection(GaussianBlurFilterBuilder::HORIZONTAL_BLUR)
                    ->blurSize((unsigned int)blurShadow)
                    ->build();

            newLightShadowMap->addTextureFilter(std::move(verticalBlurFilter));
            newLightShadowMap->addTextureFilter(std::move(horizontalBlurFilter));
        } else { //null filter necessary because it allows to store cached shadow map in a texture which is not cleared.
            std::unique_ptr<TextureFilter> nullFilter = std::make_unique<DownSampleFilterBuilder>(shadowMapTexture)
                    ->textureSize(shadowMapResolution, shadowMapResolution)
                    ->textureType(TextureType::ARRAY)
                    ->textureNumberLayer(nbShadowMaps)
                    ->textureFormat(TextureFormat::RG_32_FLOAT)
                    ->build();

            newLightShadowMap->addTextureFilter(std::move(nullFilter));
        }

        lightShadowMaps[light] = newLightShadowMap;
    }

    void ShadowManager::removeShadowLight(const Light* light) {
        delete lightShadowMaps[light];
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

    void ShadowManager::updateLightSplitsShadowMap(LightShadowMap* lightShadowMap) {
        ScopeProfiler sp(Profiler::graphic(), "upFrustumShadow");

        if (lightShadowMap->getLight()->hasParallelBeams()) { //sun light
            unsigned int i = 0;
            for (const auto& lightSplitShadowMap : lightShadowMap->getLightSplitShadowMaps()) {
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
        for (unsigned int i = 1; i <= nbShadowMaps; ++i) {
            float uniformSplit = nearDistance + (viewingShadowDistance - nearDistance) * ((float)i / (float)nbShadowMaps);
            float logarithmicSplit = nearDistance * std::pow(viewingShadowDistance / nearDistance, (float)i / (float)nbShadowMaps);

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
            lightShadowMap.second->displayModels();
            lightShadowMap.second->applyTextureFilters();
        }
    }

    void ShadowManager::loadShadowMaps(const std::unique_ptr<GenericRenderer>& lightingRenderer, std::size_t shadowMapTexUnitStart) {
        std::size_t shadowLightIndex = 0;
        for (auto* visibleLight : lightManager->getVisibleLights()) {
            if (visibleLight->isProduceShadow()) {
                assert(shadowLightIndex < getMaxShadowLights());
                const LightShadowMap* lightShadowMap = lightShadowMaps.find(visibleLight)->second;

                std::size_t shadowTexUnit = shadowMapTexUnitStart + shadowLightIndex;
                if (lightingRenderer->getTextureReader(shadowTexUnit).getTexture() != lightShadowMap->getFilteredShadowMapTexture()) {
                    lightingRenderer->updateTextureReader(shadowTexUnit, TextureReader::build(lightShadowMap->getFilteredShadowMapTexture(), TextureParam::buildLinear()));
                }

                unsigned int shadowMapIndex = 0;
                for (const auto& lightSplitShadowMap : lightShadowMap->getLightSplitShadowMaps()) {
                    std::size_t matrixIndex = shadowLightIndex * getMaxShadowLights() + shadowMapIndex;
                    lightProjectionViewMatrices[matrixIndex] = lightSplitShadowMap->getLightProjectionMatrix() * lightShadowMap->getLightViewMatrix();
                    ShaderDataSender(true).sendData(mLightProjectionViewShaderVar[shadowLightIndex][shadowMapIndex], lightProjectionViewMatrices[matrixIndex]);
                    shadowMapIndex++;
                }

                shadowLightIndex++;
            }
        }

        for (unsigned int shadowMapIndex = 0; shadowMapIndex < nbShadowMaps; ++shadowMapIndex) {
            float currSplitDistance = splitDistances[shadowMapIndex];
            depthSplitDistance[shadowMapIndex] = ((projectionMatrix(2, 2) * -currSplitDistance + projectionMatrix(2, 3)) / (currSplitDistance)) / 2.0f + 0.5f;
            //Vulkan source code:
            //depthSplitDistance[shadowMapIndex * 4 - 1] = ((projectionMatrix(2, 2) * -currSplitDistance + projectionMatrix(2, 3)) / (currSplitDistance)) / 2.0f + 0.5f;
        }

        lightingRenderer
                //Vulkan source code: ->updateShaderData(4, lightProjectionViewMatrices)
                ->updateShaderData(5, ShaderDataSender(true).sendData(depthSplitDistanceShaderVar, nbShadowMaps, depthSplitDistance));
    }

}
