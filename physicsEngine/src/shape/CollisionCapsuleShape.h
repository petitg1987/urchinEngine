#ifndef URCHINENGINE_COLLISIONCAPSULESHAPE_H
#define URCHINENGINE_COLLISIONCAPSULESHAPE_H

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

			CollisionShape3D::ShapeType getShapeType() const override;
			CollisionShape3D::ShapeCategory getShapeCategory() const override;
			std::shared_ptr<ConvexShape3D<float>> getSingleShape() const override;
			float getRadius() const;
			float getCylinderHeight() const;
			CapsuleShape<float>::CapsuleOrientation getCapsuleOrientation() const;

			std::shared_ptr<CollisionShape3D> scale(float) const override;

			AABBox<float> toAABBox(const PhysicsTransform &) const override;
			std::shared_ptr<CollisionConvexObject3D> toConvexObject(const PhysicsTransform &) const override;

			Vector3<float> computeLocalInertia(float) const override;
			float getMaxDistanceToCenter() const override;
			float getMinDistanceToCenter() const override;

		private:
			void computeSafeMargin();

			const std::shared_ptr<CapsuleShape<float>> capsuleShape; //shape including margin
	};

}

#endif
