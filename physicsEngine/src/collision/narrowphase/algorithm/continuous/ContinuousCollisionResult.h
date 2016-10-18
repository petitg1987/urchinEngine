#ifndef ENGINE_CONTINUOUSCOLLISIONRESULT_H
#define ENGINE_CONTINUOUSCOLLISIONRESULT_H

#include "UrchinCommon.h"

namespace urchin
{

	template<class T> class ContinuousCollisionResult
	{
		public:
			ContinuousCollisionResult();
			virtual ~ContinuousCollisionResult();

			virtual bool hasTimeOfImpactResult() const = 0;

			virtual const Vector3<T> &getNormal() const = 0;
			virtual const Point3<T> &getHitPointB() const = 0;

			virtual T getTimeToHit() const = 0;
	};

}

#endif
