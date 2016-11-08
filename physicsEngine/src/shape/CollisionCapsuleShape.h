#ifndef ENGINE_COLLISIONCAPSULESHAPE_H
#define ENGINE_COLLISIONCAPSULESHAPE_H

#include <memory>
#include <vector>
#include "UrchinCommon.h"

#include "shape/CollisionShape3D.h"
#include "object/CollisionConvexObject3D.h"
#include "utils/math/PhysicsTransform.h"

namespace urchin
{

	class CollisionCapsuleShape : public CollisionShape3D
	{
		public:
			CollisionCapsuleShape(float, float, CapsuleShape<float>::CapsuleOrientation);
			CollisionCapsuleShape(float, float, float, CapsuleShape<float>::CapsuleOrientation);
			~CollisionCapsuleShape();

			CollisionShape3D::ShapeType getShapeType() const;
			float getRadius() const;
			float getCylinderHeight() const;
			CapsuleShape<float>::CapsuleOrientation getCapsuleOrientation() const;

			std::shared_ptr<CollisionShape3D> scale(float) const;

			AABBox<float> toAABBox(const PhysicsTransform &) const;
			std::shared_ptr<CollisionSphereShape> toConfinedSphereShape() const;
			std::shared_ptr<CollisionConvexObject3D> toConvexObject(const PhysicsTransform &) const;

			Vector3<float> computeLocalInertia(float) const;

		private:
			void computeSafeMargin();

			const CapsuleShape<float> capsuleShape; //shape including margin
	};

}

#endif
