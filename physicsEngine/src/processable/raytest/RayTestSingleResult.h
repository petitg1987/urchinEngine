#ifndef ENGINE_RAYTESTSINGLERESULT_H
#define ENGINE_RAYTESTSINGLERESULT_H

#include "UrchinCommon.h"

#include "body/work/AbstractWorkBody.h"

namespace urchin
{

	class RayTestSingleResult
	{
		public:
			RayTestSingleResult(const Vector3<float> &, const Point3<float> &, float);
			~RayTestSingleResult();

			const Vector3<float> &getNormal() const;
			const Point3<float> &getHitPointB() const;
			float getTimeToHit() const;

		private:
			Vector3<float> normal;
			Point3<float> hitPointB;
			float timeToHit;
	};

}

#endif
