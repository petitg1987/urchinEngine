#pragma once

#include <vector>
#include <UrchinCommon.h>

namespace urchin {

    class SplitBoundingBox {
        public:
            SplitBoundingBox();

            void split(const AABBox<float>&, std::vector<AABBox<float>>&) const;

        private:
            const float limitSize;
    };

}
