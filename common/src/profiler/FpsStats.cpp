#include <profiler/FpsStats.h>

namespace urchin {

    FpsStats::FpsStats() :
        fpsMaxThresholds({4, 10, 15, 25, 40, 55, 100, 9999}),
        counts({0l, 0l, 0l, 0l, 0l, 0l, 0l, 0l}) {

    }

    void FpsStats::registerDt(float dt) {
        registerFps((unsigned int)(1.0f / dt));
    }

    void FpsStats::registerFps(unsigned int fps) {
        for (unsigned int i = 0; i < fpsMaxThresholds.size(); ++i) {
            if (fps <= fpsMaxThresholds[i]) {
                counts[i]++;
                break;
            }
        }
    }

    std::string FpsStats::toString() const {
        std::string result;

        unsigned int minFps = 0;
        for (unsigned int i = 0; i < fpsMaxThresholds.size(); ++i) {
            unsigned int maxFps = fpsMaxThresholds[i];

            result += "[" + std::to_string(minFps) + "-" + std::to_string(maxFps) + "fps]: " + std::to_string(counts[i]);
            if (i + 1 != fpsMaxThresholds.size()) {
                result += ", ";
            }

            minFps = maxFps + 1;
        }
        return result;
    }

}
