#include <scene/renderer3d/transparent/TransparentMeshFilter.h>

namespace urchin {

    bool TransparentMeshFilter::isAccepted(const Model& model) const {
        if (model.getMeshes()) {
            for (unsigned int i = 0; i < model.getMeshes()->getNumberMeshes(); ++i) {
                if (isAccepted(model.getMeshes()->getMesh(i))) {
                    return true;
                }
            }
        }
        return false;
    }

    bool TransparentMeshFilter::isAccepted(const Mesh& mesh) const {
        return mesh.getMaterial().hasTransparency();
    }

}