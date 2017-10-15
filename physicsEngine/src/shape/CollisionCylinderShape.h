#ifndef URCHINENGINE_COLLISIONCYLINDERSHAPE_H
#define URCHINENGINE_COLLISIONCYLINDERSHAPE_H

#include <memory>
#include "UrchinCommon.h"

#include "shape/CollisionShape3D.h"
#include "object/CollisionConvexObject3D.h"
#include "utils/math/PhysicsTransform.h"

namespace urchin
{

	class CollisionCylinderShape : public CollisionShape3D
	{
		public:
			CollisionCylinderShape(float, float, CylinderShape<float>::CylinderOrientation);

			CollisionShape3D::ShapeType getShapeType() const override;
			CollisionShape3D::ShapeCategory getShapeCategory() const override;
			std::shared_ptr<ConvexShape3D<float>> getSingleShape() const override;
			float getRadius() const;
			float getHeight() const;
			CylinderShape<float>::CylinderOrientation getCylinderOrientation() const;

			std::shared_ptr<CollisionShape3D> scale(float) const override;

			AABBox<float> toAABBox(const PhysicsTransform &) const override;
			std::shared_ptr<CollisionConvexObject3D> toConvexObject(const PhysicsTransform &) const override;

			Vector3<float> computeLocalInertia(float) const override;
			float getMaxDistanceToCenter() const override;
			float getMinDistanceToCenter() const override;

		private:
			void computeSafeMargin();

			const std::shared_ptr<CylinderShape<float>> cylinderShape; //shape including margin
	};

}

#endif
