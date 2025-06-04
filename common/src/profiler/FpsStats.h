#pragma once

#include <array>
#include <string>

namespace urchin {

    class FpsStats {
        public:
            FpsStats();

            void registerDt(float);
            void registerFps(unsigned int);

            std::string toString() const;

        private:
            std::array<unsigned int, 5> fpsMaxThresholds;
            std::array<long, 5> counts;
    };

}
