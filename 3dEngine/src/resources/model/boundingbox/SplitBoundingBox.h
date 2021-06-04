#pragma once

#include <vector>
#include <UrchinCommon.h>

namespace urchin {

    class SplitBoundingBox {
        public:
            SplitBoundingBox();

            std::vector<AABBox<float>> split(const AABBox<float>&) const;

        private:
            const float limitSize;
    };

}
