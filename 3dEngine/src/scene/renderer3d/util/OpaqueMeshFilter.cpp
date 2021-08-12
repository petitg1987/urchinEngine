#include <scene/renderer3d/util/OpaqueMeshFilter.h>

namespace urchin {

    bool OpaqueMeshFilter::isAccepted(const ConstMesh& constMesh) const {
        return !constMesh.getMaterial().hasTransparency();
    }

}
