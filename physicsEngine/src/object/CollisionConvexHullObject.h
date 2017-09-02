#ifndef URCHINENGINE_COLLISIONCONVEXHULLOBJECT_H
#define URCHINENGINE_COLLISIONCONVEXHULLOBJECT_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include "UrchinCommon.h"

#include "object/CollisionConvexObject3D.h"

namespace urchin
{

	class CollisionConvexHullObject : public CollisionConvexObject3D
	{
		public:
			CollisionConvexHullObject(float, const std::vector<Point3<float>> &, const std::vector<Point3<float>> &);
			CollisionConvexHullObject(float, std::shared_ptr<ConvexHull3D<float>>, std::shared_ptr<ConvexHull3D<float>>);
			~CollisionConvexHullObject();

			std::vector<Point3<float>> getPointsWithoutMargin() const;
            std::vector<Point3<float>> getPointsWithMargin() const;

			Point3<float> getSupportPoint(const Vector3<float> &, bool) const;

			std::string toString() const;

		private:
			std::shared_ptr<ConvexHull3D<float>> convexHullObjectWithMargin;
			std::shared_ptr<ConvexHull3D<float>> convexHullObjectWithoutMargin;
	};

}

#endif
