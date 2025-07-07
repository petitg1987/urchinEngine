#include <stdexcept>
#include <utility>

#include <scene/renderer3d/lighting/shadow/light/LightShadowMap.h>
#include <scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h>
#include <scene/renderer3d/lighting/shadow/display/ModelShadowSunShaderVariable.h>
#include <scene/renderer3d/lighting/shadow/display/ModelShadowOmnidirectionalShaderVariable.h>
#include <scene/renderer3d/lighting/shadow/display/ModelShadowSpotShaderVariable.h>
#include <scene/renderer3d/lighting/light/Light.h>
#include <scene/renderer3d/model/builder/ModelBuilder.h>

namespace urchin {

    LightShadowMap::LightShadowMap(bool isTestMode, Light& light, const ModelOcclusionCuller& modelOcclusionCuller, float shadowViewDistance, unsigned int shadowMapResolution, unsigned int nbShadowMaps) :
            light(light),
            modelOcclusionCuller(modelOcclusionCuller),
            shadowViewDistance(shadowViewDistance),
            shadowMapResolution(shadowMapResolution),
            defaultEmptyModel(ModelBuilder().newEmptyModel("defaultEmptyShadowModel")),
            modelsToLayersMask(EverGrowHashMap<Model*, std::bitset<8>>()) {

        if (nbShadowMaps > ShadowManager::SPLIT_SHADOW_MAPS_SHADER_LIMIT) {
            throw std::invalid_argument("Number of shadow maps must be lower than " + std::to_string(ShadowManager::SPLIT_SHADOW_MAPS_SHADER_LIMIT) + ". Value: " + std::to_string(nbShadowMaps));
        }

        for (unsigned int i = 0; i < nbShadowMaps; ++i) { //First split is the split nearest to the eye for sunlight.
            lightSplitShadowMaps.push_back(std::make_unique<LightSplitShadowMap>(i, this));
        }

        if (!isTestMode) {
            renderTarget = std::make_unique<OffscreenRender>("shadow map - " + light.getLightTypeName(), false, RenderTarget::SHARED_DEPTH_ATTACHMENT);
            renderTarget->setOutputSize(shadowMapResolution, shadowMapResolution, nbShadowMaps, true);
            renderTarget->initialize();

            std::vector variablesDescriptions = {sizeof(nbShadowMaps)};
            auto shaderConstants = std::make_unique<ShaderConstants>(variablesDescriptions, &nbShadowMaps);
            shadowModelSetDisplayer = std::make_unique<ModelSetDisplayer>(DisplayMode::DEPTH_ONLY_NO_INSTANCING_MODE); //TODO can use instance ?!
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

    Light& LightShadowMap::getLight() const {
        return light;
    }

    const ModelOcclusionCuller& LightShadowMap::getModelOcclusionCuller() const {
        return modelOcclusionCuller;
    }

    float LightShadowMap::getShadowViewDistance() const {
        return shadowViewDistance;
    }

    unsigned int LightShadowMap::getNumberShadowMaps() const {
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

        //At least one model is required to have the shadow map in correct layout (VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL)
        std::array defaultModels = {defaultEmptyModel.get()};

        modelsToLayersMask.clear();
        std::size_t layerIndex = 0;
        for (auto& lightSplitShadowMap : lightSplitShadowMaps) {
            std::span<Model* const> modelsBySplit = lightSplitShadowMap->getModels();
            for (Model* const model : modelsBySplit.empty() ? defaultModels : modelsBySplit) {
                std::bitset<8>* foundLayersMask = modelsToLayersMask.find(model);
                if (foundLayersMask == nullptr) {
                    std::bitset<8> layersMask(1 << layerIndex);
                    modelsToLayersMask.insert(model, layersMask);
                } else {
                    foundLayersMask->set(layerIndex);
                }
            }
            layerIndex++;
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
