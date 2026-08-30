#include "scene/renderer3d/lighting/shadow/light/ShadowMeshFilter.h"

namespace urchin {

    bool ShadowMeshFilter::isAccepted(const Model&) const {
        return true;
    }

    bool ShadowMeshFilter::isAccepted(const Mesh& mesh) const {
        constexpr float MAX_TRANSPARENCY_TO_CAST_SHADOW = 0.5f;
        return mesh.getMaterial().getTransparencyData().getMaxTransparency() <= MAX_TRANSPARENCY_TO_CAST_SHADOW;
    }

}
