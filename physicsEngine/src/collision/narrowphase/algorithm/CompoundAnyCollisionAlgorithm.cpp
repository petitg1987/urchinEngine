#include <limits>

#include "collision/narrowphase/algorithm/CompoundAnyCollisionAlgorithm.h"
#include "shape/CollisionCompoundShape.h"
#include "shape/CollisionShape3D.h"

namespace urchin
{

	CompoundAnyCollisionAlgorithm::CompoundAnyCollisionAlgorithm(bool objectSwapped, const ManifoldResult &result) :
			CollisionAlgorithm(objectSwapped, result),
			collisionAlgorithmSelector(new CollisionAlgorithmSelector())
	{

	}

	CompoundAnyCollisionAlgorithm::~CompoundAnyCollisionAlgorithm()
	{
		delete collisionAlgorithmSelector;
	}

	void CompoundAnyCollisionAlgorithm::doProcessCollisionAlgorithm(const CollisionObjectWrapper &object1, const CollisionObjectWrapper &object2)
	{
		const auto &compoundShape = dynamic_cast<const CollisionCompoundShape &>(object1.getShape());
		const CollisionShape3D &otherShape = object2.getShape();

		AbstractWorkBody *body1 = getManifoldResult().getBody1();
		AbstractWorkBody *body2 = getManifoldResult().getBody2();

		const std::vector<std::shared_ptr<const LocalizedCollisionShape>> &localizedShapes = compoundShape.getLocalizedShapes();
		for (const auto &localizedShape : localizedShapes)
		{
			std::shared_ptr<CollisionAlgorithm> collisionAlgorithm = collisionAlgorithmSelector->createCollisionAlgorithm(
					body1, localizedShape->shape.get(), body2, &otherShape);
			const CollisionAlgorithm *const constCollisionAlgorithm = collisionAlgorithm.get();

			PhysicsTransform shapeWorldTransform = object1.getShapeWorldTransform() * localizedShape->transform;
			CollisionObjectWrapper subObject1(*localizedShape->shape, shapeWorldTransform);
			CollisionObjectWrapper subObject2(otherShape, object2.getShapeWorldTransform());

			collisionAlgorithm->processCollisionAlgorithm(subObject1, subObject2, false);

			const ManifoldResult &algorithmManifoldResult = constCollisionAlgorithm->getConstManifoldResult();
			addContactPointsToManifold(algorithmManifoldResult, constCollisionAlgorithm->isObjectSwapped());
		}
	}

	void CompoundAnyCollisionAlgorithm::addContactPointsToManifold(const ManifoldResult &manifoldResult, bool manifoldSwapped)
	{
		for(unsigned int i=0; i<manifoldResult.getNumContactPoints(); ++i)
		{
			const ManifoldContactPoint &manifolContactPoint = manifoldResult.getManifoldContactPoint(i);
			if(manifoldSwapped)
			{
				getManifoldResult().addContactPoint(
						(-manifolContactPoint.getNormalFromObject2()),
						manifolContactPoint.getPointOnObject2(),
						manifolContactPoint.getPointOnObject1(),
						manifolContactPoint.getLocalPointOnObject2(),
						manifolContactPoint.getLocalPointOnObject1(),
						manifolContactPoint.getDepth(),
						manifolContactPoint.isPredictive());
			}else
			{
				getManifoldResult().addContactPoint(
						manifolContactPoint.getNormalFromObject2(),
						manifolContactPoint.getPointOnObject1(),
						manifolContactPoint.getPointOnObject2(),
						manifolContactPoint.getLocalPointOnObject1(),
						manifolContactPoint.getLocalPointOnObject2(),
						manifolContactPoint.getDepth(),
						manifolContactPoint.isPredictive());
			}
		}
	}

	CollisionAlgorithm *CompoundAnyCollisionAlgorithm::Builder::createCollisionAlgorithm(bool objectSwapped, const ManifoldResult &result, void* memPtr) const
	{
		return new(memPtr) CompoundAnyCollisionAlgorithm(objectSwapped, result);
	}

	std::set<CollisionShape3D::ShapeType> CompoundAnyCollisionAlgorithm::Builder::getFirstExpectedShapeType() const
	{
		return CollisionShape3D::COMPOUND_SHAPES;
	}

	unsigned int CompoundAnyCollisionAlgorithm::Builder::getAlgorithmSize() const
	{
		return sizeof(CompoundAnyCollisionAlgorithm);
	}

}
