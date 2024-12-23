#pragma once

#include <memory>
#include <mutex>

#include <collision/narrowphase/algorithm/continuous/ContinuousCollisionResult.h>

namespace urchin {

    /**
     * Result of a ray test. The result is fill asynchronously to ray test.
     */
    class RayTestResult {
        public:
            RayTestResult();

            void updateResults(const std::vector<ContinuousCollisionResult<float>>&, unsigned int);

            bool isResultReady(unsigned int) const;
            std::pair<unsigned int, std::optional<ContinuousCollisionResult<float>>> getNearestResult() const;

        private:
            mutable std::mutex mutex;

            std::atomic_uint rayTestVersion;
            std::vector<ContinuousCollisionResult<float>> rayTestResults;
    };

}
