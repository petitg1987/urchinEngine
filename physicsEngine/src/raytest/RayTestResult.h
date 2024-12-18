#pragma once

#include <atomic>
#include <memory>

#include <collision/narrowphase/algorithm/continuous/ContinuousCollisionResult.h>

namespace urchin {

    /**
     * Result of a ray test. The result is fill asynchronously to ray test.
     * Method "isResultReady" returns true when ray test result is correctly completed.
     */
    class RayTestResult {
        public:
            RayTestResult();

            void addResults(ccd_set&);

            bool isResultReady() const;

            bool hasHit() const;
            const ContinuousCollisionResult<float>& getNearestResult() const;
            const ccd_set& getResults() const;

        private:
            std::atomic_bool resultReady;

            ccd_set rayTestResults;
    };

}
