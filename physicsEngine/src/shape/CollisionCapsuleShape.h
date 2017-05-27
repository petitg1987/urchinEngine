#ifndef ENGINE_COLLISIONCAPSULESHAPE_H
#define ENGINE_COLLISIONCAPSULESHAPE_H

#include <memory>
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
			~CollisionCapsuleShape();

			CollisionShape3D::ShapeType getShapeType() const;
			std::shared_ptr<ConvexShape3D<float>> getSingleShape() const;
			float getRadius() const;
			float getCylinderHeight() const;
			CapsuleShape<float>::CapsuleOrientation getCapsuleOrientation() const;

			std::shared_ptr<CollisionShape3D> scale(float) const;

			AABBox<float> toAABBox(const PhysicsTransform &) const;
			std::shared_ptr<CollisionConvexObject3D> toConvexObject(const PhysicsTransform &) const;

			Vector3<float> computeLocalInertia(float) const;
			float getMaxDistanceToCenter() const;
			float getMinDistanceToCenter() const;

		private:
			void computeSafeMargin();

			const std::shared_ptr<CapsuleShape<float>> capsuleShape; //shape including margin
	};

}

#endif
