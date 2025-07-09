#include <scene/renderer3d/OpaqueMeshFilter.h>

namespace urchin {

    bool OpaqueMeshFilter::isAccepted(const Model& model) const {
        if (model.getMeshes()) {
            for (unsigned int i = 0; i < model.getMeshes()->getNumMeshes(); ++i) {
                if (isAccepted(model.getMeshes()->getMesh(i))) {
                    return true;
                }
            }
        }
        return false;
    }

    bool OpaqueMeshFilter::isAccepted(const Mesh& mesh) const {
        return !mesh.getMaterial().hasTransparency();
    }

}
