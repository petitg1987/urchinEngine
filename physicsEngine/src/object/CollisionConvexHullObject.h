#ifndef ENGINE_COLLISIONCONVEXHULLOBJECT_H
#define ENGINE_COLLISIONCONVEXHULLOBJECT_H

#include "UrchinCommon.h"

#include "object/CollisionConvexObject3D.h"

namespace urchin
{

	class CollisionConvexHullObject : public CollisionConvexObject3D
	{
		public:
			CollisionConvexHullObject(float, ConvexHull3D<float>, ConvexHull3D<float>);
			~CollisionConvexHullObject();

			std::vector<Point3<float>> getPointsWithoutMargin() const;

			Point3<float> getSupportPoint(const Vector3<float> &, bool) const;

			const ConvexHull3D<float> retrieveConvexHull() const;

		private:
			const ConvexHull3D<float> convexHullObjectWithMargin;
			const ConvexHull3D<float> convexHullObjectWithoutMargin;
	};

}

#endif
