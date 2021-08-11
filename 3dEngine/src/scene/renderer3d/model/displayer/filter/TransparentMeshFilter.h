#pragma once

#include <scene/renderer3d/model/displayer/filter/MeshFilter.h>

namespace urchin {

    class TransparentMeshFilter : public MeshFilter {
        public:
            bool isAccepted(const ConstMesh&) const override;
    };

}
