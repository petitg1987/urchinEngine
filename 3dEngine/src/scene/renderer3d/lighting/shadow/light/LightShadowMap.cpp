#include <stdexcept>
#include <utility>

#include "LightShadowMap.h"
#include "scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h"
#include "scene/renderer3d/lighting/shadow/display/ShadowModelShaderVariable.h"

namespace urchin {

    LightShadowMap::LightShadowMap(const Light* light, const OctreeManager<Model>* modelOctreeManager, float viewingShadowDistance, std::shared_ptr<Texture> shadowMapTexture,
                                   unsigned int nbShadowMaps, std::unique_ptr<OffscreenRender> renderTarget) :
            light(light),
            modelOctreeManager(modelOctreeManager),
            viewingShadowDistance(viewingShadowDistance),
            renderTarget(std::move(renderTarget)),
            shadowModelSetDisplayer(nullptr),
            shadowModelShaderVariable(nullptr),
            shadowMapTexture(std::move(shadowMapTexture)) {
        if (this->renderTarget) { //only false for unit tests
            createOrUpdateShadowModelSetDisplayer(nbShadowMaps);
        }
        updateLightViewMatrix();
        light->addObserver(this, Light::LIGHT_MOVE);
    }

    LightShadowMap::~LightShadowMap() {
        light->removeObserver(this, Light::LIGHT_MOVE);

        for (auto& lightSplitShadowMap : lightSplitShadowMaps) {
            delete lightSplitShadowMap;
        }

        delete shadowModelSetDisplayer;
        delete shadowModelShaderVariable;
    }

    void LightShadowMap::updateLightViewMatrix() {
        if (light->hasParallelBeams()) { //sun light
            Vector3<float> lightDirection = light->getDirections()[0];

            const Vector3<float>& f = lightDirection.normalize();
            const Vector3<float>& s = f.crossProduct(Vector3<float>(0.0f, 1.0f, 0.0f)).normalize();
            const Vector3<float>& u = s.crossProduct(f).normalize();
            Matrix4<float> M(
                    s[0],    s[1],    s[2],    0,
                    u[0],    u[1],    u[2],    0,
                    -f[0],   -f[1],   -f[2],   0,
                    0,       0,       0,       1);

            Matrix4<float> eye;
            eye.buildTranslation(lightDirection.X, lightDirection.Y, lightDirection.Z);
            Matrix4<float> mViewShadow = M * eye;

            this->lightViewMatrix = mViewShadow;
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

    const Light* LightShadowMap::getLight() const {
        return light;
    }

    const OctreeManager<Model>* LightShadowMap::getModelOctreeManager() const {
        return modelOctreeManager;
    }

    float LightShadowMap::getViewingShadowDistance() const {
        return viewingShadowDistance;
    }

    /**
     * Returns shadow map texture (variance shadow map)
     */
    const std::shared_ptr<Texture>& LightShadowMap::getShadowMapTexture() const {
        return shadowMapTexture;
    }

    void LightShadowMap::createOrUpdateShadowModelSetDisplayer(unsigned int nbShadowMaps) {
        std::map<std::string, std::string> geometryTokens, fragmentTokens;
        geometryTokens["MAX_VERTICES"] = std::to_string(3 * nbShadowMaps);
        geometryTokens["NUMBER_SHADOW_MAPS"] = std::to_string(nbShadowMaps);
        delete shadowModelSetDisplayer;
        shadowModelSetDisplayer = new ModelSetDisplayer(DisplayMode::DEPTH_ONLY_MODE);
        shadowModelSetDisplayer->setCustomGeometryShader("modelShadowMap.geom", geometryTokens);
        shadowModelSetDisplayer->setCustomFragmentShader("modelShadowMap.frag", fragmentTokens);
        shadowModelSetDisplayer->initialize(renderTarget);

        delete shadowModelShaderVariable;
        shadowModelShaderVariable = new ShadowModelShaderVariable();
        shadowModelShaderVariable->setLayersToUpdateShaderVar(shadowModelSetDisplayer->getShaderVar("layersToUpdate"));
        shadowModelShaderVariable->setProjectionMatricesShaderVar(shadowModelSetDisplayer->getShaderVar("projectionMatrix"));
        shadowModelSetDisplayer->setCustomModelShaderVariable(shadowModelShaderVariable);
    }

    /**
     * First split to add must be the split nearest to the eye.
     */
    LightSplitShadowMap* LightShadowMap::addLightSplitShadowMap() {
        auto* lightSplitShadowMap = new LightSplitShadowMap(this);
        lightSplitShadowMaps.emplace_back(lightSplitShadowMap);
        return lightSplitShadowMap;
    }

    /**
     * @return Light split shadow maps. First split in the vector is the split nearest to the eye.
     */
    const std::vector<LightSplitShadowMap*>& LightShadowMap::getLightSplitShadowMaps() const {
        return lightSplitShadowMaps;
    }

    void LightShadowMap::addTextureFilter(std::unique_ptr<TextureFilter> textureFilter) {
        textureFilters.push_back(std::move(textureFilter));
    }

    void LightShadowMap::applyTextureFilters() const {
        unsigned int layersToUpdate = retrieveLayersToUpdate();

        for (auto& textureFilter : textureFilters) {
            textureFilter->applyFilter((int)layersToUpdate);
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

    unsigned int LightShadowMap::retrieveLayersToUpdate() const {
        unsigned int layersToUpdate = 0;
        unsigned int i = 0;
        for (const auto& lightSplitShadowMap : lightSplitShadowMaps) {
            if (lightSplitShadowMap->needShadowMapUpdate()) {
                layersToUpdate = layersToUpdate | MathFunction::powerOfTwo(i);
            }
            i++;
        }
        return layersToUpdate;
    }

    void LightShadowMap::removeModel(Model* model) {
        shadowModelSetDisplayer->removeModel(model);
    }

    const std::vector<Model*>& LightShadowMap::retrieveModels() const {
        models.clear();
        for (auto &lightSplitShadowMap : lightSplitShadowMaps) {
            if (lightSplitShadowMap->needShadowMapUpdate()) {
                const std::vector<Model*>& frustumSplitModels = lightSplitShadowMap->getModels();
                OctreeableHelper<Model>::merge(models, frustumSplitModels);
            }
        }

        return models;
    }

    void LightShadowMap::displayModels() {
        renderTarget->resetDisplay();

        shadowModelShaderVariable->setLightShadowMap(this); //TODO refresh call name
        shadowModelSetDisplayer->setModels(retrieveModels());

        shadowModelSetDisplayer->display(lightViewMatrix);
    }
}
