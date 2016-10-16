#ifndef ENGINE_COLLISIONSHERESHAPE_H
#define ENGINE_COLLISIONSHERESHAPE_H

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
			~CollisionSphereShape();

			CollisionShape3D::ShapeType getShapeType() const;
			float getRadius() const;

			std::shared_ptr<CollisionShape3D> scale(float) const;
			std::shared_ptr<CollisionSphereShape> retrieveSphereShape() const;

			AABBox<float> toAABBox(const PhysicsTransform &) const;
			std::shared_ptr<CollisionConvexObject3D> toConvexObject(const PhysicsTransform &) const;

			Vector3<float> computeLocalInertia(float) const;

		private:
			const SphereShape<float> sphereShape; //shape including margin
	};

}

#endif
