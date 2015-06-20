#include <limits>

#include "collision/narrowphase/algorithm/SphereSphereCollisionAlgorithm.h"
#include "shape/CollisionSphereShape.h"

namespace urchin
{

	SphereSphereCollisionAlgorithm::SphereSphereCollisionAlgorithm(bool objectSwapped, const ManifoldResult &result) :
			CollisionAlgorithm(objectSwapped, result)
	{

	}

	SphereSphereCollisionAlgorithm::~SphereSphereCollisionAlgorithm()
	{

	}

	void SphereSphereCollisionAlgorithm::doProcessCollisionAlgorithm(const CollisionObjectWrapper &object1, const CollisionObjectWrapper &object2)
	{
		const CollisionSphereShape &sphere1 = static_cast<const CollisionSphereShape &>(object1.getShape());
		const CollisionSphereShape &sphere2 = static_cast<const CollisionSphereShape &>(object2.getShape());

		Vector3<float> diff = object2.getShapeWorldTransform().getPosition().vector(object1.getShapeWorldTransform().getPosition());
		float length = diff.length();
		float radius1 = sphere1.getRadius();
		float radius2 = sphere2.getRadius();

		if(length - getContactBreakingThreshold() < (radius1 + radius2))
		{ //collision detected
			//compute depth penetration
			float depth = length - (radius1+radius2);

			//compute normal (if both spheres have same origin: default normal is (1,0,0))
			Vector3<float> normalFromObject2(1.0, 0.0, 0.0);
			if(length > std::numeric_limits<float>::epsilon())
			{ //normalize normal (from object2 to object1)
				normalFromObject2 = diff / length;
			}

			//compute intersection point
			Point3<float> pointOnObject2 = object2.getShapeWorldTransform().getPosition().translate(radius2 * normalFromObject2);

			addNewContactPoint(normalFromObject2, pointOnObject2, depth);
		}
	}

	CollisionAlgorithm *SphereSphereCollisionAlgorithm::Builder::createCollisionAlgorithm(bool objectSwapped, const ManifoldResult &result, void* memPtr) const
	{
		return new(memPtr) SphereSphereCollisionAlgorithm(objectSwapped, result);
	}

	CollisionShape3D::ShapeType SphereSphereCollisionAlgorithm::Builder::getShapeType1() const
	{
		return CollisionShape3D::SPHERE_SHAPE;
	}

	CollisionShape3D::ShapeType SphereSphereCollisionAlgorithm::Builder::getShapeType2() const
	{
		return CollisionShape3D::SPHERE_SHAPE;
	}

	unsigned int SphereSphereCollisionAlgorithm::Builder::getAlgorithmSize() const
	{
		return sizeof(SphereSphereCollisionAlgorithm);
	}

}
