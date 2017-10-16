#ifndef URCHINENGINE_COLLISIONSHAPE3D_H
#define URCHINENGINE_COLLISIONSHAPE3D_H

#include <memory>
#include <vector>
#include <set>
#include "UrchinCommon.h"

#include "utils/math/PhysicsTransform.h"
#include "object/CollisionConvexObject3D.h"

namespace urchin
{

	class CollisionShape3D
	{
		public:
			CollisionShape3D();
			explicit CollisionShape3D(float);
			virtual ~CollisionShape3D() = default;

			enum ShapeType
			{
				SPHERE_SHAPE = 0,
				BOX_SHAPE,
				CAPSULE_SHAPE,
				CYLINDER_SHAPE,
				CONE_SHAPE,
				CONVEX_HULL_SHAPE,
				COMPOUND_SHAPE,
				HEIGHTFIELD_SHAPE,

				SHAPE_MAX
			};
            static std::set<ShapeType> CONVEX_SHAPES, CONCAVE_SHAPES, COMPOUND_SHAPES;

			float getInnerMargin() const;
			virtual CollisionShape3D::ShapeType getShapeType() const = 0;
            bool isConvex() const;
            bool isConcave() const;
            bool isCompound() const;
			virtual std::shared_ptr<ConvexShape3D<float>> getSingleShape() const = 0;

			virtual std::shared_ptr<CollisionShape3D> scale(float) const = 0;

			virtual AABBox<float> toAABBox(const PhysicsTransform &) const = 0;
			virtual std::shared_ptr<CollisionConvexObject3D> toConvexObject(const PhysicsTransform &) const = 0;

			virtual Vector3<float> computeLocalInertia(float) const = 0;
			virtual float getMaxDistanceToCenter() const = 0;
			virtual float getMinDistanceToCenter() const = 0;

			void checkInnerMarginQuality(const std::string &) const;

		protected:
			void refreshInnerMargin(float);

		private:
			float innerMargin;
			const float initialInnerMargin;
	};

}

#endif
