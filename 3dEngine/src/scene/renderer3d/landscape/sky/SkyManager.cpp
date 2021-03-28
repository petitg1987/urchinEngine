#include "SkyManager.h"

namespace urchin {
    SkyManager::SkyManager(std::shared_ptr<RenderTarget> renderTarget) :
            renderTarget(std::move(renderTarget)),
            skybox(nullptr) {

    }

    void SkyManager::onCameraProjectionUpdate(const Camera* camera) {
        projectionMatrix = camera->getProjectionMatrix();

        if (skybox != nullptr) {
            skybox->onCameraProjectionUpdate(projectionMatrix);
        }
    }

    void SkyManager::setSkybox(std::unique_ptr<Skybox> skybox) {
        this->skybox = std::move(skybox);

        if (this->skybox != nullptr) {
            this->skybox->initialize(renderTarget);
            this->skybox->onCameraProjectionUpdate(projectionMatrix);
        }
    }

    const std::unique_ptr<Skybox>& SkyManager::getSkybox() const {
        return skybox;
    }

    void SkyManager::prepareRendering(const Matrix4<float>& viewMatrix, const Point3<float>& cameraPosition) {
        if (skybox != nullptr) {
            skybox->prepareRendering(viewMatrix, cameraPosition);
        }
    }
}
