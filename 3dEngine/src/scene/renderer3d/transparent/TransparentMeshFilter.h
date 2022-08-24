#pragma once

#include <scene/renderer3d/model/displayer/MeshFilter.h>

namespace urchin {

    class TransparentMeshFilter final : public MeshFilter {
        public:
            bool isAccepted(const Model&) const override;
            bool isAccepted(const Mesh&) const override;
    };

}
