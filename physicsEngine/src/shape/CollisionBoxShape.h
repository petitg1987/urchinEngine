#ifndef ENGINE_COLLISIONBOXSHAPE_H
#define ENGINE_COLLISIONBOXSHAPE_H

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
			CollisionBoxShape(const Vector3<float> &);
			CollisionBoxShape(float, const Vector3<float> &);
			~CollisionBoxShape();

			CollisionShape3D::ShapeType getShapeType() const;
			float getHalfSize(unsigned int) const;
			const Vector3<float> &getHalfSizes() const;

			std::shared_ptr<CollisionShape3D> scale(float) const;

			AABBox<float> toAABBox(const PhysicsTransform &) const;
			std::shared_ptr<CollisionConvexObject3D> toConvexObject(const PhysicsTransform &) const;

			Vector3<float> computeLocalInertia(float) const;
			float getLargestDistance() const;
			float getSmallestDistance() const;

		private:
			void computeSafeMargin();

			const BoxShape<float> boxShape; //shape including margin
	};

}

#endif
