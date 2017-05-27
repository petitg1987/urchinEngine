#ifndef ENGINE_COLLISIONCYLINDERSHAPE_H
#define ENGINE_COLLISIONCYLINDERSHAPE_H

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
			~CollisionCylinderShape();

			CollisionShape3D::ShapeType getShapeType() const;
			std::shared_ptr<ConvexShape3D<float>> getSingleShape() const;
			float getRadius() const;
			float getHeight() const;
			CylinderShape<float>::CylinderOrientation getCylinderOrientation() const;

			std::shared_ptr<CollisionShape3D> scale(float) const;

			AABBox<float> toAABBox(const PhysicsTransform &) const;
			std::shared_ptr<CollisionConvexObject3D> toConvexObject(const PhysicsTransform &) const;

			Vector3<float> computeLocalInertia(float) const;
			float getMaxDistanceToCenter() const;
			float getMinDistanceToCenter() const;

		private:
			void computeSafeMargin();

			const std::shared_ptr<CylinderShape<float>> cylinderShape; //shape including margin
	};

}

#endif
