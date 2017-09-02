#ifndef URCHINENGINE_CONTINUOUSCOLLISIONRESULT_H
#define URCHINENGINE_CONTINUOUSCOLLISIONRESULT_H

#include <set>
#include <memory>
#include "UrchinCommon.h"

#include "body/work/AbstractWorkBody.h"

namespace urchin
{

	template<class T> class ContinuousCollisionResult
	{
		public:
			ContinuousCollisionResult(AbstractWorkBody *, const Vector3<T> &, const Point3<T> &, T);
			~ContinuousCollisionResult();

			AbstractWorkBody *getBody2() const;

			const Vector3<T> &getNormalFromObject2() const;
			const Point3<T> &getHitPointOnObject2() const;
			T getTimeToHit() const;

		private:
			AbstractWorkBody *body2;

			Vector3<T> normalFromObject2;
			Point3<T> hitPointOnObject2;
			T timeToHit;
	};

	template<class T> struct ContinuousCollisionResultComparator
	{
		bool operator()(std::shared_ptr<ContinuousCollisionResult<T>>, std::shared_ptr<ContinuousCollisionResult<T>>) const;
	};

	typedef std::set<std::shared_ptr<ContinuousCollisionResult<float>>, ContinuousCollisionResultComparator<float>> ccd_set;

}

#endif
