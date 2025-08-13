#include <stdexcept>
#include <utility>

#include "scene/renderer3d/lighting/shadow/light/LightShadowMap.h"
#include "scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h"
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
            modelsToLayersMask(EverGrowHashMap<Model*, std::bitset<8>>()) {

        if (nbSplitShadowMaps > ShadowManager::MAX_SPLIT_SHADOW_MAPS) {
            throw std::invalid_argument("Number of split shadow maps must be lower than " + std::to_string(ShadowManager::MAX_SPLIT_SHADOW_MAPS) + ". Value: " + std::to_string(nbSplitShadowMaps));
        }

        for (unsigned int i = 0; i < nbSplitShadowMaps; ++i) { //First split is the split nearest to the eye for sunlight.
            lightSplitShadowMaps.push_back(std::make_unique<LightSplitShadowMap>(i, this));
        }

        if (!isTestMode) { //TODO avoid renderTarget / shadowModelSetDisplayer for each light !
            renderTarget = std::make_unique<OffscreenRender>("shadow map - " + light.getLightTypeName(), false, RenderTarget::SHARED_DEPTH_ATTACHMENT);
            renderTarget->setOutputSize(shadowMapResolution, shadowMapResolution, nbSplitShadowMaps, true);
            renderTarget->initialize();

            std::vector variablesDescriptions = {sizeof(nbSplitShadowMaps)};
            auto shaderConstants = std::make_unique<ShaderConstants>(variablesDescriptions, &nbSplitShadowMaps);
            shadowModelSetDisplayer = std::make_unique<ModelSetDisplayer>(DisplayMode::DEPTH_ONLY_MODE);
            if (light.getLightType() == Light::LightType::SUN) {
                shadowModelSetDisplayer->setupShader("modelShadowMapSun.vert.spv", "modelShadowMap.frag.spv", std::move(shaderConstants));
                shadowModelSetDisplayer->initialize(*renderTarget);
                shadowModelSetDisplayer->setupCustomShaderVariable(std::make_unique<ModelShadowSunShaderVariable>(this));
                shadowModelSetDisplayer->setupLayerIndexDataInShader(true);
            } else if (light.getLightType() == Light::LightType::OMNIDIRECTIONAL) {
                shadowModelSetDisplayer->setupShader("modelShadowMapOmnidirectional.vert.spv", "modelShadowMap.frag.spv", std::move(shaderConstants));
                shadowModelSetDisplayer->initialize(*renderTarget);
                shadowModelSetDisplayer->setupCustomShaderVariable(std::make_unique<ModelShadowOmnidirectionalShaderVariable>(this));
                shadowModelSetDisplayer->setupLayerIndexDataInShader(true);
            } else if (light.getLightType() == Light::LightType::SPOT) {
                shadowModelSetDisplayer->setupShader("modelShadowMapSpot.vert.spv", "modelShadowMap.frag.spv", std::move(shaderConstants));
                shadowModelSetDisplayer->initialize(*renderTarget);
                shadowModelSetDisplayer->setupCustomShaderVariable(std::make_unique<ModelShadowSpotShaderVariable>(this));
                shadowModelSetDisplayer->setupLayerIndexDataInShader(false);
            } else {
                throw std::runtime_error("Unknown light type to setup shadow model set displayer: " + std::to_string((int)light.getLightType()));
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

    void LightShadowMap::removeModel(Model* model) const {
        shadowModelSetDisplayer->removeModel(model);
    }

    void LightShadowMap::updateVisibleModels() {
        ScopeProfiler sp(Profiler::graphic(), "smUpModels");

        shadowModelSetDisplayer->cleanAllModels();

        modelsToLayersMask.clear();
        std::size_t layerIndex = 0;
        for (auto& lightSplitShadowMap : lightSplitShadowMaps) {
            std::span<Model* const> modelsBySplit = lightSplitShadowMap->retrieveVisibleModels();

            for (Model* const model : modelsBySplit) {
                std::bitset<8>* foundLayersMask = modelsToLayersMask.find(model);
                if (foundLayersMask == nullptr) {
                    modelsToLayersMask.insert(model, std::bitset<8>(1 << layerIndex));
                } else {
                    foundLayersMask->set(layerIndex);
                }
            }
            layerIndex++;
        }

        if (modelsToLayersMask.isEmpty()) { //At least one model is required to have the shadow map in correct layout (VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL)
            modelsToLayersMask.insert(defaultEmptyModel.get(), std::bitset<8>(1 << 1));
        }

        for (const auto& modelToLayersMask : modelsToLayersMask) {
            shadowModelSetDisplayer->addNewModel(modelToLayersMask.key, modelToLayersMask.value);
        }
    }

    void LightShadowMap::renderModels(uint32_t frameCount, unsigned int numDependenciesToShadowMaps, unsigned int renderingOrder) const {
        renderTarget->disableAllProcessors();
        shadowModelSetDisplayer->prepareRendering(renderingOrder, Matrix4<float>() /* not used for shadow */);
        renderTarget->render(frameCount, numDependenciesToShadowMaps);
    }
}
