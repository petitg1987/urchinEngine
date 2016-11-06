#ifndef ENGINE_COLLISIONCOMPOUNDSHAPE_H
#define ENGINE_COLLISIONCOMPOUNDSHAPE_H

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
			~CollisionCompoundShape();

			CollisionShape3D::ShapeType getShapeType() const;

			std::shared_ptr<CollisionShape3D> scale(float) const;
			std::shared_ptr<CollisionSphereShape> retrieveSphereShape() const;

			AABBox<float> toAABBox(const PhysicsTransform &) const;
			std::shared_ptr<CollisionConvexObject3D> toConvexObject(const PhysicsTransform &) const;

			Vector3<float> computeLocalInertia(float) const;

			const std::vector<std::shared_ptr<const LocalizedCollisionShape>> &getLocalizedShapes() const;

		private:
			void initializeSphareShape();

			const std::vector<std::shared_ptr<const LocalizedCollisionShape>> localizedShapes;

			std::shared_ptr<CollisionSphereShape> sphereShape; //sphere including all compound shapes
	};

}

#endif
