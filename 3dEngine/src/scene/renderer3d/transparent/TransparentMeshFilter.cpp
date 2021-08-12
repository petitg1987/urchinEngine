#include <scene/renderer3d/transparent/TransparentMeshFilter.h>

namespace urchin {

    bool TransparentMeshFilter::isAccepted(const ConstMesh& constMesh) const {
        return constMesh.getMaterial().hasTransparency();
    }

}