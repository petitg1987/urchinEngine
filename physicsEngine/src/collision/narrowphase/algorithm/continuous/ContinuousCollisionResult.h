#ifndef ENGINE_CONTINUOUSCOLLISIONRESULT_H
#define ENGINE_CONTINUOUSCOLLISIONRESULT_H

#include <set>
#include <memory>
#include "UrchinCommon.h"

#include "body/work/AbstractWorkBody.h"

namespace urchin
{

	template<class T> class ContinuousCollisionResult
	{
		public:
			ContinuousCollisionResult(const AbstractWorkBody *, const Vector3<T> &, const Point3<T> &, T);
			~ContinuousCollisionResult();

			const AbstractWorkBody *getBodyB() const;

			const Vector3<T> &getNormal() const;
			const Point3<T> &getHitPointB() const;
			T getTimeToHit() const;

		private:
			const AbstractWorkBody *bodyB;

			Vector3<T> normal;
			Point3<T> hitPointB;
			T timeToHit;
	};

	template<class T> struct ContinuousCollisionResultComparator
	{
		bool operator()(std::shared_ptr<ContinuousCollisionResult<T>>, std::shared_ptr<ContinuousCollisionResult<T>>) const;
	};

	typedef std::set<std::shared_ptr<ContinuousCollisionResult<float>>, ContinuousCollisionResultComparator<float>> ccd_set;

}

#endif
