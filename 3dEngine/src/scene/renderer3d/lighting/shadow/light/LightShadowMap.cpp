#include <stdexcept>
#include <utility>

#include <scene/renderer3d/lighting/shadow/light/LightShadowMap.h>
#include <scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h>
#include <scene/renderer3d/lighting/shadow/display/ModelShadowSunShaderVariable.h>
#include <scene/renderer3d/lighting/shadow/display/ModelShadowSpotShaderVariable.h>

#include "scene/renderer3d/lighting/light/Light.h"

namespace urchin {

    LightShadowMap::LightShadowMap(bool isTestMode, Light& light, const ModelOcclusionCuller& modelOcclusionCuller, float shadowViewDistance, unsigned int shadowMapResolution, unsigned int nbShadowMaps) :
            light(light),
            modelOcclusionCuller(modelOcclusionCuller),
            shadowViewDistance(shadowViewDistance),
            shadowMapResolution(shadowMapResolution),
            nbShadowMaps(nbShadowMaps) {

        if (!isTestMode) {
            renderTarget = std::make_unique<OffscreenRender>("shadow map - " + light.getLightTypeName(), false, RenderTarget::SHARED_DEPTH_ATTACHMENT);
            renderTarget->setOutputSize(shadowMapResolution, shadowMapResolution, nbShadowMaps, true);
            renderTarget->initialize();

            std::vector variablesDescriptions = {sizeof(nbShadowMaps)};
            auto shaderConstants = std::make_unique<ShaderConstants>(variablesDescriptions, &nbShadowMaps);
            shadowModelSetDisplayer = std::make_unique<ModelSetDisplayer>(DisplayMode::DEPTH_ONLY_MODE);
            if (light.getLightType() == Light::LightType::SUN) {
                shadowModelSetDisplayer->setupShader("modelShadowMapSun.vert.spv", "modelShadowMap.frag.spv", std::move(shaderConstants));
                shadowModelSetDisplayer->initialize(*renderTarget);
                shadowModelSetDisplayer->setupCustomShaderVariable(std::make_unique<ModelShadowSunShaderVariable>(this));
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

        for (unsigned int i = 0; i < nbShadowMaps; ++i) { //First split is the split nearest to the eye.
            lightSplitShadowMaps.push_back(std::make_unique<LightSplitShadowMap>(this));
        }
        updateLightViewMatrix();
        light.addObserver(this, Light::LIGHT_MOVE);
    }

    LightShadowMap::~LightShadowMap() {
        light.removeObserver(this, Light::LIGHT_MOVE);
        if (renderTarget) {
            renderTarget->cleanup();
        }
    }

    void LightShadowMap::updateLightViewMatrix() {
        if (light.getLightType() == Light::LightType::SUN || light.getLightType() == Light::LightType::SPOT) {
            Vector3<float> lightDirection = light.getDirections()[0];

            Vector3<float> forward = lightDirection.normalize();
            Vector3 worldUp(0.0f, 1.0f, 0.0f);
            if (std::abs(forward.dotProduct(worldUp)) > 0.999f) {
                worldUp = Vector3(1.0f, 0.0f, 0.0f);
            }
            Vector3<float> side = forward.crossProduct(worldUp).normalize();
            Vector3<float> up = side.crossProduct(forward).normalize();

            float translationOnSide = 0.0f;
            float translationOnUp = 0.0f;
            float translationOnForward = 0.0f;
            if (light.getLightType() == Light::LightType::SUN) {
                translationOnSide = lightDirection.X;
                translationOnUp = lightDirection.Y;
                translationOnForward = lightDirection.Z;
            } else if (light.getLightType() == Light::LightType::SPOT) {
                Vector3<float> pos = light.getPosition().toVector();
                translationOnSide = -side.dotProduct(pos);
                translationOnUp = -up.dotProduct(pos);
                translationOnForward = forward.dotProduct(pos);
            }

            this->lightViewMatrix.setValues(
                    side[0],        side[1],        side[2],        translationOnSide,
                    up[0],          up[1],          up[2],          translationOnUp,
                    -forward[0],    -forward[1],    -forward[2],    translationOnForward,
                    0.0f,           0.0f,           0.0f,           1.0f);
        } else {
            throw std::runtime_error("Shadow currently not supported for light of type: " + std::to_string((int)light.getLightType()));
        }
    }

    void LightShadowMap::notify(Observable* observable, int notificationType) {
        if (dynamic_cast<Light*>(observable)) {
            if (notificationType == Light::LIGHT_MOVE) {
                updateLightViewMatrix();
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
        return nbShadowMaps;
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

    const Matrix4<float>& LightShadowMap::getLightViewMatrix() const {
        return lightViewMatrix;
    }

    void LightShadowMap::removeModel(Model* model) const {
        shadowModelSetDisplayer->removeModel(model);
    }

    std::span<Model*> LightShadowMap::retrieveModels() const {
        models.clear();
        for (auto& lightSplitShadowMap : lightSplitShadowMaps) {
            std::span<Model* const> frustumSplitModels = lightSplitShadowMap->getModels();
            OctreeableHelper<Model>::merge(models, frustumSplitModels);
        }

        return models;
    }

    void LightShadowMap::renderModels(uint32_t frameIndex, unsigned int numDependenciesToShadowMaps, unsigned int renderingOrder) const {
        shadowModelSetDisplayer->updateModels(retrieveModels());

        renderTarget->disableAllProcessors();
        shadowModelSetDisplayer->prepareRendering(renderingOrder, lightViewMatrix);
        renderTarget->render(frameIndex, numDependenciesToShadowMaps);
    }
}
