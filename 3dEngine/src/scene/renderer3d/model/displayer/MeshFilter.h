#pragma once

#include <resources/model/ConstMesh.h>

namespace urchin {

    class MeshFilter {
        public:
            virtual ~MeshFilter() = default;

            virtual bool isAccepted(const ConstMesh&) const = 0;
    };

}