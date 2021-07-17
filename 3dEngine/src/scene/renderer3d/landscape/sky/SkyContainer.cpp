#include <scene/renderer3d/landscape/sky/SkyContainer.h>

namespace urchin {
    SkyContainer::SkyContainer(RenderTarget& renderTarget) :
            renderTarget(renderTarget),
            skybox(nullptr) {

    }

    void SkyContainer::onCameraProjectionUpdate(const Camera& camera) {
        projectionMatrix = camera.getProjectionMatrix();

        if (skybox != nullptr) {
            skybox->onCameraProjectionUpdate(projectionMatrix);
        }
    }

    void SkyContainer::setSkybox(std::unique_ptr<Skybox> skybox) {
        this->skybox = std::move(skybox);

        if (this->skybox != nullptr) {
            this->skybox->initialize(renderTarget);
            this->skybox->onCameraProjectionUpdate(projectionMatrix);
        }
    }

    const Skybox* SkyContainer::getSkybox() const {
        return skybox.get();
    }

    void SkyContainer::prepareRendering(const Matrix4<float>& viewMatrix, const Point3<float>& cameraPosition) {
        if (skybox != nullptr) {
            skybox->prepareRendering(viewMatrix, cameraPosition);
        }
    }
}
