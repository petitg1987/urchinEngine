#pragma once

#include <UrchinCommon.h>

#include <scene/renderer3d/model/Model.h>

namespace urchin {

    class ModelProduceShadowFilter {
        public:
            bool isAccepted(const Model*, const ConvexObject3D<float>&) const;
    };
}
