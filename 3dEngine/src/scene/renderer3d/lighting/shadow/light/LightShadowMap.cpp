#include <cmath>
#include <stdexcept>

#include "LightShadowMap.h"
#include "scene/renderer3d/lighting/shadow/light/LightSplitShadowMap.h"

namespace urchin {

    LightShadowMap::LightShadowMap(const Light* light, const OctreeManager<Model>* modelOctreeManager, float viewingShadowDistance) :
            light(light),
            modelOctreeManager(modelOctreeManager),
            viewingShadowDistance(viewingShadowDistance),
            renderTarget(nullptr) {
        updateLightViewMatrix();
        light->addObserver(this, Light::LIGHT_MOVE);
    }

    LightShadowMap::~LightShadowMap() {
        light->removeObserver(this, Light::LIGHT_MOVE);

        for (auto& lightSplitShadowMap : lightSplitShadowMaps) {
            delete lightSplitShadowMap;
        }
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

    void LightShadowMap::setRenderTarget(std::unique_ptr<OffscreenRender>&& renderTarget) {
        this->renderTarget = std::move(renderTarget);
    }

    const OffscreenRender* LightShadowMap::getRenderTarget() const {
        return renderTarget.get();
    }

    void LightShadowMap::setDepthTexture(const std::shared_ptr<Texture>& depthTexture) {
        this->depthTexture = depthTexture;
    }

    const std::shared_ptr<Texture>& LightShadowMap::getDepthTexture() const {
        return depthTexture;
    }

    /**
     * @param shadowMapTexture Shadow map texture (variance shadow map)
     */
    void LightShadowMap::setShadowMapTexture(const std::shared_ptr<Texture>& shadowMapTexture) {
        this->shadowMapTexture = shadowMapTexture;
    }

    /**
     * Returns shadow map texture (variance shadow map)
     */
    const std::shared_ptr<Texture>& LightShadowMap::getShadowMapTexture() const {
        return shadowMapTexture;
    }

    void LightShadowMap::addTextureFilter(std::unique_ptr<TextureFilter> textureFilter) {
        textureFilters.push_back(std::move(textureFilter));
    }

    void LightShadowMap::applyTextureFilters() const {
        unsigned int layersToUpdate = retrieveLayersToUpdate();

        std::shared_ptr<Texture> texture = shadowMapTexture;
        for (auto& textureFilter : textureFilters) {
            textureFilter->applyOn(texture, (int)layersToUpdate);
            texture = textureFilter->getTexture();
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
                layersToUpdate = layersToUpdate | MathFunction::instance()->powerOfTwo(i);
            }
            i++;
        }
        return layersToUpdate;
    }

    const std::vector<Model*>& LightShadowMap::retrieveModels() const {
        models.clear();
        for (auto &lightSplitShadowMap : lightSplitShadowMaps) {
            if (lightSplitShadowMap->needShadowMapUpdate()) {
                const std::vector<Model*>& frustumSplitModels = lightSplitShadowMap->getModels();
                OctreeableHelper<Model>().merge(models, frustumSplitModels);
            }
        }

        return models;
    }
}
