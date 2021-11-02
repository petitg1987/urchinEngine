#include <algorithm>

#include <resources/geometry/GeometryContainer.h>

namespace urchin {

    GeometryContainer::GeometryContainer(RenderTarget& renderTarget) :
            renderTarget(renderTarget) {

    }

    void GeometryContainer::addGeometry(std::shared_ptr<GeometryModel> geometry) {
        geometry->initialize(renderTarget);

        geometryModels.push_back(std::move(geometry));
    }

    void GeometryContainer::removeGeometry(const GeometryModel& geometry) {
        auto it = std::find_if(geometryModels.begin(), geometryModels.end(), [&geometry](const auto& o){return o.get() == &geometry;});
        if (it != geometryModels.end()) {
            geometryModels.erase(it);
        }
    }

    void GeometryContainer::prepareRendering(unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix) const {
        for (auto& geometryModel : geometryModels) {
            geometryModel->prepareRendering(renderingOrder, projectionViewMatrix);
        }
    }

}
