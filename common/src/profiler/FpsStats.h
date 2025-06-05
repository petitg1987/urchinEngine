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
            constexpr static std::size_t NUM_THRESHOLDS = 8;
            std::array<unsigned int, NUM_THRESHOLDS> fpsMaxThresholds;
            std::array<long, NUM_THRESHOLDS> counts;
    };

}
