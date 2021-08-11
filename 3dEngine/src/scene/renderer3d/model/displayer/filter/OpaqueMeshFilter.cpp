#include <scene/renderer3d/model/displayer/filter/OpaqueMeshFilter.h>

namespace urchin {

    bool OpaqueMeshFilter::isAccepted(const ConstMesh& constMesh) const {
        return !constMesh.getMaterial().hasTransparency();;
    }

}
