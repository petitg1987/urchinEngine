#include <raytest/RayTestResult.h>

namespace urchin {

    RayTestResult::RayTestResult() {
        rayTestVersion.store(0);
    }

    void RayTestResult::updateResults(const std::vector<ContinuousCollisionResult<float>>& rayTestResults, unsigned int rayTestVersion) {
        {
            std::lock_guard lock(mutex);
            this->rayTestResults = rayTestResults;
        }
        this->rayTestVersion.store(rayTestVersion, std::memory_order_relaxed);
    }
    
    bool RayTestResult::isResultReady(unsigned int minRayTestResultVersion) const {
        return rayTestVersion.load(std::memory_order_acquire) >= minRayTestResultVersion;
    }

    std::pair<unsigned int, std::optional<ContinuousCollisionResult<float>>> RayTestResult::getNearestResult() const {
        unsigned int rayTestVersionLocal = rayTestVersion.load(std::memory_order_acquire);
        if (rayTestVersionLocal == 0) {
            throw std::runtime_error("Ray test result is not ready.");
        }

        std::lock_guard lock(mutex);
        if (rayTestResults.empty()) {
            return std::make_pair(rayTestVersionLocal, std::nullopt);
        }
        const ContinuousCollisionResult<float> &nearestResult = *std::ranges::min_element(rayTestResults, ContinuousCollisionResultComparator<float>());
        return std::make_pair(rayTestVersionLocal, nearestResult);
    }

}
