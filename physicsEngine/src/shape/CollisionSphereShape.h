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
			explicit CollisionSphereShape(float);

			CollisionShape3D::ShapeType getShapeType() const override;
			std::shared_ptr<ConvexShape3D<float>> getSingleShape() const override;
			float getRadius() const;

			std::shared_ptr<CollisionShape3D> scale(float) const override;

			AABBox<float> toAABBox(const PhysicsTransform &) const override;
			std::shared_ptr<CollisionConvexObject3D> toConvexObject(const PhysicsTransform &) const override;

			Vector3<float> computeLocalInertia(float) const override;
			float getMaxDistanceToCenter() const override;
			float getMinDistanceToCenter() const override;

			CollisionShape3D *clone() const override;

		private:
			const std::shared_ptr<SphereShape<float>> sphereShape; //shape including margin
	};

}

#endif
