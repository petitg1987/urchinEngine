#include <stdexcept>
#include <utility>
#include <cstring>

#include "scene/renderer3d/lighting/shadow/light/LightShadowMap.h"
#include "scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h"
#include "scene/renderer3d/lighting/shadow/light/ShadowMeshFilter.h"
#include "scene/renderer3d/lighting/shadow/display/ModelShadowSunShaderVariable.h"
#include "scene/renderer3d/lighting/shadow/display/ModelShadowOmnidirectionalShaderVariable.h"
#include "scene/renderer3d/lighting/shadow/display/ModelShadowSpotShaderVariable.h"
#include "scene/renderer3d/lighting/light/Light.h"
#include "scene/renderer3d/model/builder/ModelBuilder.h"

namespace urchin {

    LightShadowMap::LightShadowMap(bool isTestMode, Light& light, const ModelOcclusionCuller& modelOcclusionCuller, float shadowViewDistance, unsigned int shadowMapResolution, unsigned int nbSplitShadowMaps) :
            light(light),
            modelOcclusionCuller(modelOcclusionCuller),
            shadowViewDistance(shadowViewDistance),
            shadowMapResolution(shadowMapResolution),
            defaultEmptyModel(ModelBuilder().newEmptyModel("defaultEmptyShadowModel")),
            currentModelsToLayersMask(EverGrowHashMap<Model*, std::bitset<8>>()),
            shadowMapOutdated(true),
            displayedModelsToLayersMask(EverGrowHashMap<Model*, std::bitset<8>>()) {

        if (nbSplitShadowMaps > ShadowManager::SPLIT_SHADOW_MAPS_SHADER_LIMIT) {
            throw std::invalid_argument("Number of split shadow maps must be <= " + std::to_string(ShadowManager::SPLIT_SHADOW_MAPS_SHADER_LIMIT) + ". Value: " + std::to_string(nbSplitShadowMaps));
        }

        for (unsigned int i = 0; i < nbSplitShadowMaps; ++i) { //First split is the split nearest to the eye for sunlight.
            lightSplitShadowMaps.push_back(std::make_unique<LightSplitShadowMap>(i, this));
        }

        if (!isTestMode) {
            renderTarget = std::make_unique<OffscreenRender>("SM - " + light.getLightTypeName() + " (" + light.getName() + ")", false, RenderTarget::SHARED_DEPTH_ATTACHMENT);
            renderTarget->setOutputSize(shadowMapResolution, shadowMapResolution, nbSplitShadowMaps, true);
            renderTarget->initialize();

            std::vector variablesDescriptions = {sizeof(nbSplitShadowMaps)};
            auto shaderConstants = std::make_unique<ShaderConstants>(variablesDescriptions, &nbSplitShadowMaps);
            shadowModelSetDisplayer = std::make_unique<ModelSetDisplayer>(DisplayMode::DEPTH_ONLY_MODE);
            shadowModelSetDisplayer->setupMeshFilter(std::make_unique<ShadowMeshFilter>());
            if (light.getLightType() == Light::LightType::SUN) {
                shadowModelSetDisplayer->setupShader("modelShadowMapSun.vert.spv", "modelShadowMap.frag.spv", std::move(shaderConstants));
                shadowModelSetDisplayer->setupCustomShaderVariable(std::make_unique<ModelShadowSunShaderVariable>(this));
                shadowModelSetDisplayer->setupLayerIndexDataInShader(true);
                shadowModelSetDisplayer->initialize(*renderTarget);
            } else if (light.getLightType() == Light::LightType::OMNIDIRECTIONAL) {
                shadowModelSetDisplayer->setupShader("modelShadowMapOmnidirectional.vert.spv", "modelShadowMap.frag.spv", std::move(shaderConstants));
                shadowModelSetDisplayer->setupCustomShaderVariable(std::make_unique<ModelShadowOmnidirectionalShaderVariable>(this));
                shadowModelSetDisplayer->setupLayerIndexDataInShader(true);
                shadowModelSetDisplayer->initialize(*renderTarget);
            } else if (light.getLightType() == Light::LightType::SPOT) {
                shadowModelSetDisplayer->setupShader("modelShadowMapSpot.vert.spv", "modelShadowMap.frag.spv", std::move(shaderConstants));
                shadowModelSetDisplayer->setupCustomShaderVariable(std::make_unique<ModelShadowSpotShaderVariable>(this));
                shadowModelSetDisplayer->setupLayerIndexDataInShader(false);
                shadowModelSetDisplayer->initialize(*renderTarget);
            } else {
                throw std::runtime_error("Unknown light type to setup shadow model set displayer: " + std::to_string((int)light.getLightType()));
            }
        }
    }

    LightShadowMap::~LightShadowMap() {
        for (const auto& displayedModelToLayersMask : displayedModelsToLayersMask) {
            unobserveModel(*displayedModelToLayersMask.key);
        }
    }

    void LightShadowMap::notify(Observable* observable, int notificationType) {
        if (dynamic_cast<Model*>(observable)) {
            if (notificationType == Model::TRANSFORM_UPDATED
                    || notificationType == Model::MESH_VERTICES_UPDATED
                    || notificationType == Model::MATERIAL_UPDATED
                    || notificationType == Model::MODEL_PROPERTIES_UPDATED) {
                shadowMapOutdated = true;
            }
        }
    }

    void LightShadowMap::onLightAffectedZoneUpdated() const {
        for (const auto& lightSplitShadowMap : lightSplitShadowMaps) {
            lightSplitShadowMap->onLightAffectedZoneUpdated();
        }
    }

    void LightShadowMap::onSplitFrustumUpdated(const std::vector<SplitFrustum>& splitFrustums) const {
        assert(splitFrustums.size() == lightSplitShadowMaps.size());
        for (std::size_t i = 0; i < lightSplitShadowMaps.size(); ++i) {
            lightSplitShadowMaps[i]->onSplitFrustumUpdated(splitFrustums[i]);
        }
    }

    Light& LightShadowMap::getLight() const {
        return light;
    }

    const ModelOcclusionCuller& LightShadowMap::getModelOcclusionCuller() const {
        return modelOcclusionCuller;
    }

    float LightShadowMap::getShadowViewDistance() const {
        return shadowViewDistance;
    }

    unsigned int LightShadowMap::getNumberSplitShadowMaps() const {
        return (unsigned int)lightSplitShadowMaps.size();
    }

    unsigned int LightShadowMap::getShadowMapSize() const {
        return shadowMapResolution;
    }

    const std::shared_ptr<Texture>& LightShadowMap::getShadowMapTexture() const {
        return renderTarget->getDepthTexture();
    }

    /**
     * @return Light split shadow maps. First split in the vector is the split nearest to the eye.
     */
    const std::vector<std::unique_ptr<LightSplitShadowMap>>& LightShadowMap::getLightSplitShadowMaps() const {
        return lightSplitShadowMaps;
    }

    void LightShadowMap::removeModel(Model* model) {
        shadowModelSetDisplayer->unregisterModel(model);

        if (displayedModelsToLayersMask.erase(model)) {
            unobserveModel(*model);
            shadowMapOutdated = true;
        }
    }

    void LightShadowMap::updateVisibleModels() {
        ScopeProfiler sp(Profiler::graphic(), "smUpModels");

        currentModelsToLayersMask.clear();
        std::size_t layerIndex = 0;
        for (auto& lightSplitShadowMap : lightSplitShadowMaps) {
            std::span<Model* const> modelsBySplit = lightSplitShadowMap->retrieveVisibleModels();

            for (Model* const model : modelsBySplit) {
                std::bitset<8>* foundLayersMask = currentModelsToLayersMask.find(model);
                if (foundLayersMask == nullptr) {
                    currentModelsToLayersMask.insert(model, std::bitset<8>(1 << layerIndex));
                } else {
                    foundLayersMask->set(layerIndex);
                }
            }
            layerIndex++;
        }

        if (currentModelsToLayersMask.isEmpty()) { //At least one model is required to have the shadow map in correct layout (VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL)
            currentModelsToLayersMask.insert(defaultEmptyModel.get(), std::bitset<8>(255));
        }

        if (shadowMapOutdated || hasShadowCastersChanged()) {
            updateShadowCasters();

            shadowModelSetDisplayer->resetModelsToDisplay();
            for (const auto& modelToLayersMask : displayedModelsToLayersMask) {
                shadowModelSetDisplayer->addModelToDisplay(modelToLayersMask.key, modelToLayersMask.value);
            }

            shadowMapOutdated = true;
        }
    }

    bool LightShadowMap::hasShadowCastersChanged() {
        if (currentModelsToLayersMask.getSize() != displayedModelsToLayersMask.getSize()) {
            return true;
        }

        for (const auto& modelToLayersMask : currentModelsToLayersMask) {
            const std::bitset<8>* displayedLayersMask = displayedModelsToLayersMask.find(modelToLayersMask.key);
            if (displayedLayersMask == nullptr || *displayedLayersMask != modelToLayersMask.value) {
                return true;
            }
        }
        return false;
    }

    void LightShadowMap::updateShadowCasters() {
        for (const auto& displayedModelToLayersMask : displayedModelsToLayersMask) {
            if (currentModelsToLayersMask.find(displayedModelToLayersMask.key) == nullptr) {
                unobserveModel(*displayedModelToLayersMask.key);
            }
        }
        for (const auto& currentModelToLayersMask : currentModelsToLayersMask) {
            if (displayedModelsToLayersMask.find(currentModelToLayersMask.key) == nullptr) {
                observeModel(*currentModelToLayersMask.key);
            }
        }

        displayedModelsToLayersMask = std::move(currentModelsToLayersMask);
    }

    bool LightShadowMap::hasLightProjectionViewMatricesChanged() const {
        if (renderedLightProjectionViewMatrices.size() != lightSplitShadowMaps.size()) {
            return true;
        }

        for (std::size_t i = 0; i < lightSplitShadowMaps.size(); ++i) {
            if (std::memcmp(&renderedLightProjectionViewMatrices[i], &lightSplitShadowMaps[i]->getLightProjectionViewMatrix(), sizeof(Matrix4<float>)) != 0) {
                return true;
            }
        }
        return false;
    }

    void LightShadowMap::observeModel(Model& model) {
        model.addObserver(this, Model::TRANSFORM_UPDATED);
        model.addObserver(this, Model::MESH_VERTICES_UPDATED);
        model.addObserver(this, Model::MATERIAL_UPDATED);
        model.addObserver(this, Model::MODEL_PROPERTIES_UPDATED);
    }

    void LightShadowMap::unobserveModel(Model& model) {
        model.removeObserver(this, Model::MODEL_PROPERTIES_UPDATED);
        model.removeObserver(this, Model::MATERIAL_UPDATED);
        model.removeObserver(this, Model::MESH_VERTICES_UPDATED);
        model.removeObserver(this, Model::TRANSFORM_UPDATED);
    }

    void LightShadowMap::renderModels(uint32_t frameCount, unsigned int numDependenciesToShadowMaps, unsigned int renderingOrder) {
        if (!shadowMapOutdated && !hasLightProjectionViewMatricesChanged()) {
            return;
        }

        renderTarget->disableAllProcessors();
        shadowModelSetDisplayer->prepareRendering(renderingOrder, Matrix4<float>() /* not used for shadow */);
        renderTarget->render(frameCount, numDependenciesToShadowMaps);

        shadowMapOutdated = false;
        renderedLightProjectionViewMatrices.clear();
        for (const std::unique_ptr<LightSplitShadowMap>& lightSplitShadowMap : lightSplitShadowMaps) {
            renderedLightProjectionViewMatrices.push_back(lightSplitShadowMap->getLightProjectionViewMatrix());
        }
    }
}
