#pragma once

#include <resources/model/ConstMesh.h>
#include <scene/renderer3d/model/Model.h>

namespace urchin {

    class MeshFilter {
        public:
            virtual ~MeshFilter() = default;

            virtual bool isAccepted(const Model&) const = 0;
            virtual bool isAccepted(const ConstMesh&) const = 0;
    };

}
