#include <cmath>
#include <stdexcept>

#include "ShadowData.h"
#include "scene/renderer3d/lighting/shadow/data/FrustumShadowData.h"

namespace urchin {

    ShadowData::ShadowData(const Light* light, const OctreeManager<Model>* modelOctreeManager, float viewingShadowDistance) :
            light(light),
            modelOctreeManager(modelOctreeManager),
            viewingShadowDistance(viewingShadowDistance),
            renderTarget(nullptr) {
        updateLightViewMatrix();
        light->addObserver(this, Light::LIGHT_MOVE);
    }

    ShadowData::~ShadowData() {
        light->removeObserver(this, Light::LIGHT_MOVE);

        for (auto& fsd : frustumShadowData) {
            delete fsd;
        }
    }

    void ShadowData::updateLightViewMatrix() {
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

    void ShadowData::notify(Observable* observable, int notificationType) {
        if (auto* light = dynamic_cast<Light*>(observable)) {
            if (notificationType == Light::LIGHT_MOVE) {
                assert(light == this->light);
                updateLightViewMatrix();
            }
        }
    }

    void ShadowData::addFrustumShadowData() { //TODO add comment. First must be the nearest !
        frustumShadowData.emplace_back(new FrustumShadowData(this));
    }

    const Light* ShadowData::getLight() const {
        return light;
    }

    const OctreeManager<Model>* ShadowData::getModelOctreeManager() const {
        return modelOctreeManager;
    }

    float ShadowData::getViewingShadowDistance() const {
        return viewingShadowDistance;
    }

    void ShadowData::setRenderTarget(std::unique_ptr<OffscreenRender>&& renderTarget) {
        this->renderTarget = std::move(renderTarget);
    }

    const OffscreenRender* ShadowData::getRenderTarget() const {
        return renderTarget.get();
    }

    void ShadowData::setDepthTexture(const std::shared_ptr<Texture>& depthTexture) {
        this->depthTexture = depthTexture;
    }

    const std::shared_ptr<Texture>& ShadowData::getDepthTexture() const {
        return depthTexture;
    }

    /**
     * @param shadowMapTexture Shadow map texture (variance shadow map)
     */
    void ShadowData::setShadowMapTexture(const std::shared_ptr<Texture>& shadowMapTexture) {
        this->shadowMapTexture = shadowMapTexture;
    }

    /**
     * Returns shadow map texture (variance shadow map)
     */
    const std::shared_ptr<Texture>& ShadowData::getShadowMapTexture() const {
        return shadowMapTexture;
    }

    void ShadowData::addTextureFilter(std::unique_ptr<TextureFilter> textureFilter) {
        textureFilters.push_back(std::move(textureFilter));
    }

    void ShadowData::applyTextureFilters() const {
        unsigned int layersToUpdate = retrieveLayersToUpdate();

        std::shared_ptr<Texture> texture = shadowMapTexture;
        for (auto& textureFilter : textureFilters) {
            textureFilter->applyOn(texture, (int)layersToUpdate);
            texture = textureFilter->getTexture();
        }
    }

    const std::shared_ptr<Texture>& ShadowData::getFilteredShadowMapTexture() const {
        if (textureFilters.empty()) {
            return shadowMapTexture;
        }

        return textureFilters.back()->getTexture();
    }

    const Matrix4<float>& ShadowData::getLightViewMatrix() const {
        return lightViewMatrix;
    }

    const std::vector<FrustumShadowData*>& ShadowData::getFrustumShadowData() const {
        return frustumShadowData;
    }

    unsigned int ShadowData::retrieveLayersToUpdate() const {
        unsigned int layersToUpdate = 0;
        unsigned int i = 0;
        for(const auto& frustumShadowData : getFrustumShadowData()) {
            if (frustumShadowData->needShadowMapUpdate()) {
                layersToUpdate = layersToUpdate | MathFunction::powerOfTwo(i);
            }
            i++;
        }
        return layersToUpdate;
    }

    const std::vector<Model*>& ShadowData::retrieveModels() const {
        models.clear();
        for(auto &fsd : frustumShadowData) {
            if (fsd->needShadowMapUpdate()) {
                const std::vector<Model*>& frustumSplitModels =fsd->getModels();
                OctreeableHelper<Model>::merge(models, frustumSplitModels);
            }
        }

        return models;
    }
}
