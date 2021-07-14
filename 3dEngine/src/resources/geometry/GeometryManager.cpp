#include <algorithm>

#include <resources/geometry/GeometryManager.h>

namespace urchin {

    GeometryManager::GeometryManager(RenderTarget &renderTarget) :
            renderTarget(renderTarget) {

    }

    void GeometryManager::addGeometry(std::shared_ptr<GeometryModel> geometry) {
        geometry->initialize(renderTarget);
        geometry->onCameraProjectionUpdate(projectionMatrix);

        geometryModels.push_back(std::move(geometry));
    }

    void GeometryManager::removeGeometry(const GeometryModel& geometry) {
        auto it = std::find_if(geometryModels.begin(), geometryModels.end(), [&geometry](const auto& o){return o.get() == &geometry;});
        if (it != geometryModels.end()) {
            geometryModels.erase(it);
        }
    }

    void GeometryManager::onCameraProjectionUpdate(const Camera& camera) {
        this->projectionMatrix = camera.getProjectionMatrix();

        for (auto& geometryModel : geometryModels) {
            geometryModel->onCameraProjectionUpdate(projectionMatrix);
        }
    }

    void GeometryManager::prepareRendering(const Matrix4<float>& viewMatrix) const {
        for (auto& geometryModel : geometryModels) {
            geometryModel->prepareRendering(viewMatrix);
        }
    }

}
