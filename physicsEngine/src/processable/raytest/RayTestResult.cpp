#include <stdexcept>

#include "processable/raytest/RayTestResult.h"

namespace urchin {

    RayTestResult::RayTestResult() :
            resultReady(false) {

    }

    void RayTestResult::addResults(ccd_set& rayTestResults) {
        assert(this->rayTestResults.empty());

        this->rayTestResults.merge(rayTestResults);

        resultReady.store(true, std::memory_order_relaxed);
    }

    /**
     * Return true if result is available. Indeed, after calling test ray method, the result is not directly available
     * as the physics engine work in separate thread.
     */
    bool RayTestResult::isResultReady() const {
        return resultReady.load(std::memory_order_relaxed);
    }

    bool RayTestResult::hasHit() const {
        if (!resultReady.load(std::memory_order_relaxed)) {
            throw std::runtime_error("Ray test callback result is not ready.");
        }

        return !rayTestResults.empty();
    }

    const std::unique_ptr<ContinuousCollisionResult<float>, AlgorithmResultDeleter>& RayTestResult::getNearestResult() const {
        if (!resultReady.load(std::memory_order_relaxed)) {
            throw std::runtime_error("Ray test callback result is not ready.");
        }

        assert(!rayTestResults.empty());

        return *rayTestResults.begin();
    }

    const ccd_set& RayTestResult::getResults() const {
        if (!resultReady.load(std::memory_order_relaxed)) {
            throw std::runtime_error("Ray test callback result is not ready.");
        }

        return rayTestResults;
    }

}
