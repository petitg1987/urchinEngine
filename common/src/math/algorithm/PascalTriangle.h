#pragma once

#include <vector>

namespace urchin {

    class PascalTriangle {
        public:
            PascalTriangle() = delete;

            static std::vector<unsigned int> lineValues(unsigned int);
    };

}
