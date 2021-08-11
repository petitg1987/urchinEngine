#include <scene/renderer3d/model/displayer/filter/TransparentMeshFilter.h>

namespace urchin {

    bool TransparentMeshFilter::isAccepted(const ConstMesh& constMesh) const {
        return constMesh.getMaterial().hasTransparency();
    }

}