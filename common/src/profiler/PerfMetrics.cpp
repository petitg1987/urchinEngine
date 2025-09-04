#include "profiler/PerfMetrics.h"
#include "math/algorithm/MathFunction.h"

namespace urchin {

    PerfMetrics::PerfMetrics() :
        msThresholds({10.0f, 18.0f, 25.0f, 40.0f, 65.0f, 100.0f, 250.0f}),
        counts({0l, 0l, 0l, 0l, 0l, 0l, 0l, 0l}) {

    }

    void PerfMetrics::registerDt(float dtInSec) {
        float dtInMs = dtInSec * 1000.0f;
        for (unsigned int i = 0; i < msThresholds.size(); ++i) {
            if (dtInMs <= msThresholds[i]) {
                counts[i]++;
                break;
            }
        }
    }

    std::string PerfMetrics::toString() const {
        std::string result;

        float minMs = 0.0f;
        for (unsigned int i = 0; i < msThresholds.size() + 1; ++i) {
            float maxMs = (i == msThresholds.size()) ? std::numeric_limits<float>::max() : msThresholds[i];

            std::string minMsString = std::to_string(MathFunction::roundToInt(minMs));
            std::string maxMsString = (maxMs == std::numeric_limits<float>::max()) ? "∞" : std::to_string(MathFunction::roundToInt(maxMs));

            result += "[" + minMsString + "-" + maxMsString + "ms]: " + std::to_string(counts[i]);
            if (i != msThresholds.size()) {
                result += ", ";
            }

            minMs = maxMs;
        }
        return result;
    }

}
