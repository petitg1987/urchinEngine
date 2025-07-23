#include <algorithm>

#include "resources/geometry/GeometryContainer.h"

namespace urchin {

    GeometryContainer::GeometryContainer(RenderTarget& renderTarget) :
            renderTarget(renderTarget) {

    }

    void GeometryContainer::addGeometry(std::shared_ptr<GeometryModel> geometry) {
        geometry->initialize(renderTarget);

        geometryModels.push_back(std::move(geometry));
    }

    void GeometryContainer::removeGeometry(const GeometryModel& geometry) {
        auto it = std::ranges::find_if(geometryModels, [&geometry](const auto& o){return o.get() == &geometry;});
        if (it != geometryModels.end()) {
            geometryModels.erase(it);
        }
    }

    void GeometryContainer::prepareRendering(unsigned int& renderingOrder, const Matrix4<float>& projectionViewMatrix) const {
        for (auto& geometryModel : geometryModels) {
            renderingOrder++;
            geometryModel->prepareRendering(renderingOrder, projectionViewMatrix);
        }
    }

}
