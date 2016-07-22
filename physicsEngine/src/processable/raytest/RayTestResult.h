#ifndef ENGINE_RAYTESTRESULT_H
#define ENGINE_RAYTESTRESULT_H

#include <mutex>
#include <atomic>
#include "UrchinCommon.h"

#include "processable/raytest/RayTestSingleResult.h"

namespace urchin
{

	/**
	 * Result of a ray test. The result is fill asynchronously to ray test.
	 * Method "isResultReady" returns true when ray test result is correctly completed.
	 */
	class RayTestResult
	{
		public:
			RayTestResult();
			~RayTestResult();

			void addResults(const std::vector<RayTestSingleResult> &);

			bool isResultReady() const;

			bool hasHit() const;
			const RayTestSingleResult &getNearestResult() const;
			const std::vector<RayTestSingleResult> &getResults() const;

		private:
			std::atomic_bool resultReady;

			std::vector<RayTestSingleResult> rayTestResults;
			unsigned int nearestResultIndex;

	};

}

#endif
