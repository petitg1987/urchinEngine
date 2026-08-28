#include "scene/renderer3d/lighting/shadow/light/ShadowMeshFilter.h"

namespace urchin {

    bool ShadowMeshFilter::isAccepted(const Model&) const {
        return true;
    }

    bool ShadowMeshFilter::isAccepted(const Mesh& mesh) const {
        return !mesh.getMaterial().hasTransparency(); //TODO impl: transparancy > 50%
    }

}
