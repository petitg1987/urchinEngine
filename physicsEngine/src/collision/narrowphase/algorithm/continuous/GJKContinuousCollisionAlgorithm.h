#ifndef ENGINE_GJKCONTINUOUSCOLLISIONALGORITHM_H
#define ENGINE_GJKCONTINUOUSCOLLISIONALGORITHM_H

#include <memory>

#include "body/work/AbstractWorkBody.h"
#include "object/TemporalObject.h"
#include "collision/narrowphase/algorithm/continuous/ContinuousCollisionResult.h"

namespace urchin
{

	/**
	* Implementation of GJK Ray Cast algorithm (see http://www.dtecta.com/papers/unpublished04raycast.pdf)
	*/
	template<class T, class OUT> class GJKContinuousCollisionAlgorithm
	{
		public:
			GJKContinuousCollisionAlgorithm();
			~GJKContinuousCollisionAlgorithm();

			std::shared_ptr<ContinuousCollisionResult<OUT>> calculateTimeOfImpact(const TemporalObject &, const TemporalObject &, AbstractWorkBody *) const;

		private:
			Point3<T> getWorldSupportPoint(const TemporalObject &, const Vector3<T> &, const PhysicsTransform &) const;
			Point3<float> interpolate(const Point3<float> &, const Point3<float> &, T) const;
			#ifdef _DEBUG
				void logMaximumIterationReach() const;
			#endif

			const T squareEpsilon;
			const unsigned int maxIteration;
			const float terminationTolerance;
	};

}

#endif
