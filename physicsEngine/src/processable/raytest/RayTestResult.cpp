#include <stdexcept>
#include <limits>

#include "processable/raytest/RayTestResult.h"

namespace urchin
{

	RayTestResult::RayTestResult() :
		nearestResultIndex(0)
	{
		resultReady.store(false, std::memory_order_relaxed);
	}

	RayTestResult::~RayTestResult()
	{

	}

	void RayTestResult::addResults(const std::vector<RayTestSingleResult> &rayTestResults)
	{
		this->rayTestResults = rayTestResults;

		float nearestTimeToHit = std::numeric_limits<float>::max();
		for(unsigned int i=0; i<rayTestResults.size(); ++i)
		{
			if(rayTestResults[i].getTimeToHit() < nearestTimeToHit)
			{
				nearestTimeToHit = rayTestResults[i].getTimeToHit();
				nearestResultIndex = i;
			}
		}

		resultReady.store(true, std::memory_order_relaxed);
	}

	/**
	 * Return true if result is available. Indeed, after calling test ray method, the result is not directly available
	 * as the physics engine work in separate thread.
	 */
	bool RayTestResult::isResultReady() const
	{
		return resultReady.load(std::memory_order_relaxed);
	}

	bool RayTestResult::hasHit() const
	{
		if(!resultReady.load(std::memory_order_relaxed))
		{
			throw std::runtime_error("Ray test callback result is not ready.");
		}

		return rayTestResults.size() > 0;
	}

	const RayTestSingleResult &RayTestResult::getNearestResult() const
	{
		if(!resultReady.load(std::memory_order_relaxed))
		{
			throw std::runtime_error("Ray test callback result is not ready.");
		}

		if(!hasHit())
		{
			throw std::runtime_error("Impossible to return nearest result: there is no result.");
		}

		return rayTestResults[nearestResultIndex];
	}

	const std::vector<RayTestSingleResult> &RayTestResult::getResults() const
	{
		if(!resultReady.load(std::memory_order_relaxed))
		{
			throw std::runtime_error("Ray test callback result is not ready.");
		}

		return rayTestResults;
	}

}
