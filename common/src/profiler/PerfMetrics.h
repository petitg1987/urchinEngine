#pragma once

#include <array>
#include <string>

namespace urchin {

    class PerfMetrics {
        public:
            PerfMetrics();

            void registerDt(float);

            std::string toString() const;

        private:
            constexpr static std::size_t NUM_THRESHOLDS = 7;
            std::array<float, NUM_THRESHOLDS> msThresholds;
            std::array<long, NUM_THRESHOLDS + 1> counts;
    };

}
