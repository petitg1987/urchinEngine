#include <scene/renderer3d/util/OpaqueMeshFilter.h>

namespace urchin {

    bool OpaqueMeshFilter::isAccepted(const Model& model) const {
        const std::vector<std::unique_ptr<const ConstMesh>>& constMeshes = model.getConstMeshes()->getConstMeshes();
        return std::any_of(constMeshes.begin(), constMeshes.end(), [&](const auto& constMesh) {
            return isAccepted(*constMesh);
        });
    }

    bool OpaqueMeshFilter::isAccepted(const ConstMesh& constMesh) const {
        return !constMesh.getMaterial().hasTransparency();
    }

}
