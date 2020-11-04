#include "SkyManager.h"

namespace urchin {
    SkyManager::SkyManager() :
        skybox(nullptr) {

    }

    void SkyManager::onCameraProjectionUpdate(const Camera *camera) {
        projectionMatrix = camera->getProjectionMatrix();

        if (skybox != nullptr) {
            skybox->onCameraProjectionUpdate(projectionMatrix);
        }
    }

    void SkyManager::setSkybox(std::unique_ptr<Skybox> skybox) {
        this->skybox = std::move(skybox);

        if (this->skybox != nullptr) {
            this->skybox->onCameraProjectionUpdate(projectionMatrix);
        }
    }

    const std::unique_ptr<Skybox> &SkyManager::getSkybox() const {
        return skybox;
    }

    void SkyManager::display(const Matrix4<float> &viewMatrix, const Point3<float> &cameraPosition) {
        if (skybox != nullptr) {
            skybox->display(viewMatrix, cameraPosition);
        }
    }
}
