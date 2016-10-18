#ifndef ENGINE_CONTINUOUSCOLLISIONRESULTTOI_H
#define ENGINE_CONTINUOUSCOLLISIONRESULTTOI_H

#include "collision/narrowphase/algorithm/continuous/ContinuousCollisionResult.h"

namespace urchin
{

	template<class T> class ContinuousCollisionResultTOI : public ContinuousCollisionResult<T>
	{
		public:
			ContinuousCollisionResultTOI(const Vector3<T> &, const Point3<T> &, T);
			~ContinuousCollisionResultTOI();

			bool hasTimeOfImpactResult() const;

			const Vector3<T> &getNormal() const;
			const Point3<T> &getHitPointB() const;

			T getTimeToHit() const;

		private:
			Vector3<T> normal;
			Point3<T> hitPointB;

			T timeToHit;
	};

}

#endif
