#ifndef ENGINE_GJKRAYCASTALGORITHM_H
#define ENGINE_GJKRAYCASTALGORITHM_H

#include <memory>

#include "collision/narrowphase/algorithm/raycast/RayCastObject.h"
#include "collision/narrowphase/algorithm/raycast/RayCastResult.h"

namespace urchin
{

	/**
	* Implementation of GJK Ray Cast algorithm (see http://www.dtecta.com/papers/unpublished04raycast.pdf)
	*/
	template<class T> class GJKRayCastAlgorithm
	{
		public:
			GJKRayCastAlgorithm();
			~GJKRayCastAlgorithm();

			std::shared_ptr<RayCastResult<T>> calculateTimeOfImpact(const RayCastObject &, const RayCastObject &) const;

		private:
			Point3<T> getWorldSupportPoint(const RayCastObject &, const Vector3<T> &, const PhysicsTransform &) const;
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
