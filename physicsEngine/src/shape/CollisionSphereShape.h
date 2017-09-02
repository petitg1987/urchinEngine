#ifndef URCHINENGINE_COLLISIONSHERESHAPE_H
#define URCHINENGINE_COLLISIONSHERESHAPE_H

#include <memory>
#include <vector>
#include "UrchinCommon.h"

#include "shape/CollisionShape3D.h"
#include "object/CollisionConvexObject3D.h"
#include "utils/math/PhysicsTransform.h"

namespace urchin
{

	class CollisionSphereShape : public CollisionShape3D
	{
		public:
			CollisionSphereShape(float);

			CollisionShape3D::ShapeType getShapeType() const;
			std::shared_ptr<ConvexShape3D<float>> getSingleShape() const;
			float getRadius() const;

			std::shared_ptr<CollisionShape3D> scale(float) const;

			AABBox<float> toAABBox(const PhysicsTransform &) const;
			std::shared_ptr<CollisionConvexObject3D> toConvexObject(const PhysicsTransform &) const;

			Vector3<float> computeLocalInertia(float) const;
			float getMaxDistanceToCenter() const;
			float getMinDistanceToCenter() const;

		private:
			const std::shared_ptr<SphereShape<float>> sphereShape; //shape including margin
	};

}

#endif
