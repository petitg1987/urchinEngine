#pragma once

#include "math/geometry/3d/MeshData.h"

namespace urchin {

    class MeshSimplificationService {
        public:
            MeshData simplify(const MeshData&) const;

        private:
            MeshData mergeDuplicateVertices(const MeshData&) const;
    };

}
