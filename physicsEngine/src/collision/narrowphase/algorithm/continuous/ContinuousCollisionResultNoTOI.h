#ifndef ENGINE_CONTINUOUSCOLLISIONRESULTNOTOI_H
#define ENGINE_CONTINUOUSCOLLISIONRESULTNOTOI_H

#include "collision/narrowphase/algorithm/continuous/ContinuousCollisionResult.h"

namespace urchin
{

	template<class T> class ContinuousCollisionResultNoTOI : public ContinuousCollisionResult<T>
	{
		public:
			ContinuousCollisionResultNoTOI();
			~ContinuousCollisionResultNoTOI();

			bool hasTimeOfImpactResult() const;

			const Vector3<T> &getNormal() const;
			const Point3<T> &getHitPointB() const;

			T getTimeToHit() const;
	};

}

#endif
