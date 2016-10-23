#include <stdexcept>

#include "processable/raytest/RayTestResult.h"

namespace urchin
{

	RayTestResult::RayTestResult()
	{
		resultReady.store(false, std::memory_order_relaxed);
	}

	RayTestResult::~RayTestResult()
	{

	}

	void RayTestResult::addResults(const ccd_set &rayTestResults)
	{
		this->rayTestResults = rayTestResults;

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

	std::shared_ptr<ContinuousCollisionResult<float>> RayTestResult::getNearestResult() const
	{
		if(!resultReady.load(std::memory_order_relaxed))
		{
			throw std::runtime_error("Ray test callback result is not ready.");
		}

		#ifdef _DEBUG
			if(!hasHit())
			{
				throw std::runtime_error("Impossible to return nearest result: there is no result.");
			}
		#endif

		return *rayTestResults.begin();
	}

	const ccd_set &RayTestResult::getResults() const
	{
		if(!resultReady.load(std::memory_order_relaxed))
		{
			throw std::runtime_error("Ray test callback result is not ready.");
		}

		return rayTestResults;
	}

}
