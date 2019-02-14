#ifndef URCHINENGINE_COLLISIONBOXSHAPE_H
#define URCHINENGINE_COLLISIONBOXSHAPE_H

#include <memory>
#include <vector>
#include "UrchinCommon.h"

#include "shape/CollisionShape3D.h"
#include "object/CollisionConvexObject3D.h"
#include "utils/math/PhysicsTransform.h"

namespace urchin
{

	class CollisionBoxShape : public CollisionShape3D
	{
		public:
			explicit CollisionBoxShape(const Vector3<float> &);
			CollisionBoxShape(CollisionBoxShape &&) noexcept;
			CollisionBoxShape(const CollisionBoxShape &) = delete;
			~CollisionBoxShape() override;

			CollisionShape3D::ShapeType getShapeType() const override;
			const ConvexShape3D<float> *getSingleShape() const override;
			float getHalfSize(unsigned int) const;
			const Vector3<float> &getHalfSizes() const;

			std::shared_ptr<CollisionShape3D> scale(float) const override;

			AABBox<float> toAABBox(const PhysicsTransform &) const override;
			std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> toConvexObject(const PhysicsTransform &) const override;

			Vector3<float> computeLocalInertia(float) const override;
			float getMaxDistanceToCenter() const override;
			float getMinDistanceToCenter() const override;

			CollisionShape3D *clone() const override;

		private:
			void computeSafeMargin();

			BoxShape<float> *boxShape; //shape including margin
	};

}

#endif
