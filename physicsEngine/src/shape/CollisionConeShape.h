#ifndef ENGINE_COLLISIONCONESHAPE_H
#define ENGINE_COLLISIONCONESHAPE_H

#include <memory>
#include "UrchinCommon.h"

#include "shape/CollisionShape3D.h"
#include "object/CollisionConvexObject3D.h"
#include "utils/math/PhysicsTransform.h"

namespace urchin
{

	class CollisionConeShape : public CollisionShape3D
	{
		public:
			CollisionConeShape(float, float, ConeShape<float>::ConeOrientation);
			~CollisionConeShape();

			CollisionShape3D::ShapeType getShapeType() const;
			std::shared_ptr<ConvexShape3D<float>> getSingleShape() const;
			float getRadius() const;
			float getHeight() const;
			ConeShape<float>::ConeOrientation getConeOrientation() const;

			std::shared_ptr<CollisionShape3D> scale(float) const;

			AABBox<float> toAABBox(const PhysicsTransform &) const;
			std::shared_ptr<CollisionConvexObject3D> toConvexObject(const PhysicsTransform &) const;

			Vector3<float> computeLocalInertia(float) const;
			float getMaxDistanceToCenter() const;
			float getMinDistanceToCenter() const;

		private:
			void computeSafeMargin();

			const std::shared_ptr<ConeShape<float>> coneShape; //shape including margin

	};

}

#endif
