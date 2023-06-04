#include <stdexcept>
#include <utility>

#include <scene/renderer3d/lighting/shadow/light/LightShadowMap.h>
#include <scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h>
#include <scene/renderer3d/lighting/shadow/display/ShadowModelShaderVariable.h>

namespace urchin {

    LightShadowMap::LightShadowMap(Light& light, const ModelOcclusionCuller& modelOcclusionCuller, float viewingShadowDistance, std::shared_ptr<Texture> shadowMapTexture,
                                   unsigned int nbShadowMaps, std::unique_ptr<OffscreenRender> renderTarget) :
            light(light),
            modelOcclusionCuller(modelOcclusionCuller),
            viewingShadowDistance(viewingShadowDistance),
            nbShadowMaps(nbShadowMaps),
            renderTarget(std::move(renderTarget)),
            shadowMapTexture(std::move(shadowMapTexture)) {
        if (this->renderTarget) { //only false for unit tests
            this->renderTarget->initialize();
            createOrUpdateShadowModelSetDisplayer(nbShadowMaps);
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
        if (light.getLightType() == Light::LightType::SUN) {
            Vector3<float> lightDirection = light.getDirections()[0];

            const Vector3<float>& f = lightDirection.normalize();
            const Vector3<float>& s = f.crossProduct(Vector3<float>(0.0f, 1.0f, 0.0f)).normalize();
            const Vector3<float>& u = s.crossProduct(f).normalize();
            Matrix4<float> m(
                    s[0],    s[1],    s[2],    0,
                    u[0],    u[1],    u[2],    0,
                    -f[0],   -f[1],   -f[2],   0,
                    0,       0,       0,       1);

            Matrix4<float> eye = Matrix4<float>::buildTranslation(lightDirection.X, lightDirection.Y, lightDirection.Z);
            this->lightViewMatrix = m * eye;
        } else {
            throw std::runtime_error("Shadow currently not supported on omnidirectional light.");
        }
    }

    void LightShadowMap::notify(Observable* observable, int notificationType) {
        if (dynamic_cast<Light*>(observable)) {
            if (notificationType == Light::LIGHT_MOVE) {
                updateLightViewMatrix();
            }
        }
    }

    const Light& LightShadowMap::getLight() const {
        return light;
    }

    const ModelOcclusionCuller& LightShadowMap::getModelOcclusionCuller() const {
        return modelOcclusionCuller;
    }

    float LightShadowMap::getViewingShadowDistance() const {
        return viewingShadowDistance;
    }

    unsigned int LightShadowMap::getNumberShadowMaps() const {
        return nbShadowMaps;
    }

    void LightShadowMap::createOrUpdateShadowModelSetDisplayer(unsigned int nbShadowMaps) {
        assert(renderTarget);
        std::vector<std::size_t> variablesDescriptions = {sizeof(nbShadowMaps)};
        auto shaderConstants = std::make_unique<ShaderConstants>(variablesDescriptions, &nbShadowMaps);

        shadowModelSetDisplayer = std::make_unique<ModelSetDisplayer>(DisplayMode::DEPTH_ONLY_MODE);
        shadowModelSetDisplayer->setupShader("modelDepthOnly.vert.spv", "modelShadowMap.geom.spv", "modelShadowMap.frag.spv", std::move(shaderConstants));
        shadowModelSetDisplayer->initialize(*renderTarget);
        shadowModelSetDisplayer->setupCustomShaderVariable(std::make_unique<ShadowModelShaderVariable>(this));
    }

    /**
     * First split to add must be the split nearest to the eye.
     */
    LightSplitShadowMap& LightShadowMap::addLightSplitShadowMap() {
        lightSplitShadowMaps.push_back(std::make_unique<LightSplitShadowMap>(this));
        assert(lightSplitShadowMaps.size() <= nbShadowMaps);
        return *lightSplitShadowMaps.back();
    }

    /**
     * @return Light split shadow maps. First split in the vector is the split nearest to the eye.
     */
    const std::vector<std::unique_ptr<LightSplitShadowMap>>& LightShadowMap::getLightSplitShadowMaps() const {
        return lightSplitShadowMaps;
    }

    void LightShadowMap::addTextureFilter(std::unique_ptr<TextureFilter> textureFilter) {
        textureFilters.push_back(std::move(textureFilter));
    }

    bool LightShadowMap::hasTextureFilter() const {
        return !textureFilters.empty();
    }

    void LightShadowMap::applyTextureFilters(std::uint32_t frameIndex, unsigned int numDependenciesToOutput) const {
        for (std::size_t i = 0; i < textureFilters.size(); ++i) {
            if (i == textureFilters.size() - 1) {
                unsigned int numDependenciesToFilterOutputs = numDependenciesToOutput;
                textureFilters[i]->applyFilter(frameIndex, numDependenciesToFilterOutputs);
            } else {
                unsigned int numDependenciesToFilterOutputs = 1 /*next filter */;
                textureFilters[i]->applyFilter(frameIndex, numDependenciesToFilterOutputs);
            }
        }
    }

    const std::shared_ptr<Texture>& LightShadowMap::getFilteredShadowMapTexture() const {
        if (textureFilters.empty()) {
            return shadowMapTexture;
        }

        return textureFilters.back()->getTexture();
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

    void LightShadowMap::renderModels(std::uint32_t frameIndex, unsigned int numDependenciesToRawShadowMaps, unsigned int renderingOrder) const {
        shadowModelSetDisplayer->updateModels(retrieveModels());

        renderTarget->disableAllProcessors();
        shadowModelSetDisplayer->prepareRendering(renderingOrder, lightViewMatrix);
        renderTarget->render(frameIndex, numDependenciesToRawShadowMaps);
    }
}
