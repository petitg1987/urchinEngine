#include <stdexcept>

#include <raytest/RayTestResult.h>

namespace urchin {

    RayTestResult::RayTestResult() :
            resultReady(false) {

    }

    void RayTestResult::addResults(ccd_set& rayTestResults) {
        assert(this->rayTestResults.empty());

        this->rayTestResults.merge(rayTestResults);

        resultReady.store(true, std::memory_order_release);
    }

    /**
     * Return true if result is available. Indeed, after calling test ray method, the result is not directly available
     * as the physics engine work in separate thread.
     */
    bool RayTestResult::isResultReady() const {
        return resultReady.load(std::memory_order_acquire);
    }

    bool RayTestResult::hasHit() const {
        if (!resultReady.load(std::memory_order_acquire)) {
            throw std::runtime_error("Ray test callback result is not ready.");
        }

        return !rayTestResults.empty();
    }

    const ContinuousCollisionResult<float>& RayTestResult::getNearestResult() const {
        if (!resultReady.load(std::memory_order_acquire)) {
            throw std::runtime_error("Ray test callback result is not ready.");
        }

        assert(!rayTestResults.empty());
        return *rayTestResults.begin();
    }

    const ccd_set& RayTestResult::getResults() const {
        if (!resultReady.load(std::memory_order_acquire)) {
            throw std::runtime_error("Ray test callback result is not ready.");
        }
        return rayTestResults;
    }

}
