#include <scene/renderer3d/landscape/sky/SkyContainer.h>

namespace urchin {
    SkyContainer::SkyContainer(RenderTarget& renderTarget) :
            renderTarget(renderTarget),
            skybox(nullptr) {

    }

    void SkyContainer::setSkybox(std::unique_ptr<Skybox> skybox) {
        ScopeProfiler sp(Profiler::graphic(), "setupSkybox");

        this->skybox = std::move(skybox);

        if (this->skybox != nullptr) {
            this->skybox->initialize(renderTarget);
        }
    }

    const Skybox* SkyContainer::getSkybox() const {
        return skybox.get();
    }

    void SkyContainer::prepareRendering(unsigned int renderingOrder, const Matrix4<float>& projectionViewMatrix, const Point3<float>& cameraPosition) {
        if (skybox != nullptr) {
            skybox->prepareRendering(renderingOrder, projectionViewMatrix, cameraPosition);
        }
    }
}
