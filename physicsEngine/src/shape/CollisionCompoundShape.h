#ifndef URCHINENGINE_COLLISIONCOMPOUNDSHAPE_H
#define URCHINENGINE_COLLISIONCOMPOUNDSHAPE_H

#include <memory>
#include <vector>
#include "UrchinCommon.h"

#include "shape/CollisionShape3D.h"
#include "object/CollisionConvexObject3D.h"
#include "utils/math/PhysicsTransform.h"

namespace urchin
{
	struct LocalizedCollisionShape
	{
		unsigned int position;

		std::shared_ptr<const CollisionShape3D> shape;
		PhysicsTransform transform;
	};

	class CollisionCompoundShape : public CollisionShape3D
	{
		public:
			CollisionCompoundShape(const std::vector<std::shared_ptr<const LocalizedCollisionShape>> &);

			CollisionShape3D::ShapeType getShapeType() const;
			std::shared_ptr<ConvexShape3D<float>> getSingleShape() const;
			const std::vector<std::shared_ptr<const LocalizedCollisionShape>> &getLocalizedShapes() const;

			std::shared_ptr<CollisionShape3D> scale(float) const;

			AABBox<float> toAABBox(const PhysicsTransform &) const;
			std::shared_ptr<CollisionConvexObject3D> toConvexObject(const PhysicsTransform &) const;

			Vector3<float> computeLocalInertia(float) const;
			float getMaxDistanceToCenter() const;
			float getMinDistanceToCenter() const;

		private:
			void initializeDistances();

			const std::vector<std::shared_ptr<const LocalizedCollisionShape>> localizedShapes;

			float maxDistanceToCenter;
			float minDistanceToCenter;
	};

}

#endif
