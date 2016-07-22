#ifndef ENGINE_RAYCASTRESULTTOI_H
#define ENGINE_RAYCASTRESULTTOI_H

#include "collision/narrowphase/algorithm/raycast/RayCastResult.h"

namespace urchin
{

	template<class T> class RayCastResultTOI : public RayCastResult<T>
	{
		public:
			RayCastResultTOI(const Vector3<T> &, const Point3<T> &, T);
			~RayCastResultTOI();

			bool hasTimeOfImpactResult() const;

			const Vector3<T> getNormal() const;

			const Point3<T> getHitPointA() const;
			const Point3<T> getHitPointB() const;

			const T getTimeToHit() const;

		private:
			Vector3<T> normal;
			Point3<T> hitPointB;

			T timeToHit;
	};

}

#endif
