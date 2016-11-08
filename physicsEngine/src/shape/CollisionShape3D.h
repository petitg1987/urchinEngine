#ifndef ENGINE_COLLISIONSHAPE3D_H
#define ENGINE_COLLISIONSHAPE3D_H

#include <memory>
#include <vector>
#include "UrchinCommon.h"

#include "utils/math/PhysicsTransform.h"
#include "object/CollisionConvexObject3D.h"

namespace urchin
{
	class CollisionSphereShape;

	class CollisionShape3D
	{
		public:
			CollisionShape3D();
			CollisionShape3D(float);
			virtual ~CollisionShape3D();

			enum ShapeType
			{
				ANY_TYPE = -1,

				SPHERE_SHAPE = 0,
				BOX_SHAPE,
				CAPSULE_SHAPE,
				CYLINDER_SHAPE,
				CONVEX_HULL_SHAPE,
				COMPOUND_SHAPE,

				SHAPE_MAX
			};

			float getInnerMargin() const;

			virtual CollisionShape3D::ShapeType getShapeType() const = 0;

			virtual std::shared_ptr<CollisionShape3D> scale(float) const = 0;

			virtual AABBox<float> toAABBox(const PhysicsTransform &) const = 0;
			virtual std::shared_ptr<CollisionSphereShape> toConfinedSphereShape() const = 0;
			virtual std::shared_ptr<CollisionConvexObject3D> toConvexObject(const PhysicsTransform &) const = 0;

			virtual Vector3<float> computeLocalInertia(float) const = 0;

			#ifdef _DEBUG
				void checkInnerMarginQuality(const std::string &) const;
			#endif

		protected:
			void refreshInnerMargin(float);

		private:
			float innerMargin;
			const float initialInnerMargin;
	};

}

#endif
