#ifndef ENGINE_RAYCASTRESULTNOTOI_H
#define ENGINE_RAYCASTRESULTNOTOI_H

#include "collision/narrowphase/algorithm/raycast/RayCastResult.h"

namespace urchin
{

	template<class T> class RayCastResultNoTOI : public RayCastResult<T>
	{
		public:
			RayCastResultNoTOI();
			~RayCastResultNoTOI();

			bool hasTimeOfImpactResult() const;

			const Vector3<T> getNormal() const;
			const Point3<T> getHitPointB() const;

			const T getTimeToHit() const;
	};

}

#endif
