#include <limits>

#include "collision/narrowphase/algorithm/CompoundCollisionAlgorithm.h"
#include "shape/CollisionCompoundShape.h"
#include "shape/CollisionShape3D.h"

namespace urchin
{

	CompoundCollisionAlgorithm::CompoundCollisionAlgorithm(bool objectSwapped, const ManifoldResult &result) :
			CollisionAlgorithm(objectSwapped, result)
	{
		collisionAlgorithmSelector = new CollisionAlgorithmSelector();
	}

	CompoundCollisionAlgorithm::~CompoundCollisionAlgorithm()
	{
		delete collisionAlgorithmSelector;
	}

	void CompoundCollisionAlgorithm::doProcessCollisionAlgorithm(const CollisionObjectWrapper &object1, const CollisionObjectWrapper &object2)
	{
		const CollisionCompoundShape &compoundShape = static_cast<const CollisionCompoundShape &>(object1.getShape());
		const CollisionShape3D &otherShape = object2.getShape();

		AbstractWorkBody *body1 = getManifoldResult().getBody1();
		AbstractWorkBody *body2 = getManifoldResult().getBody2();

		const std::vector<std::shared_ptr<const LocalizedCollisionShape>> &localizedShapes = compoundShape.getLocalizedShapes();
		for(unsigned int i=0; i<localizedShapes.size(); ++i)
		{
			const std::shared_ptr<const LocalizedCollisionShape> &localizedShape = localizedShapes[i];
			std::shared_ptr<CollisionAlgorithm> collisionAlgorithm = collisionAlgorithmSelector->createCollisionAlgorithm(
					body1, localizedShape->shape.get(), body2, &otherShape);
			const CollisionAlgorithm *const constCollisionAlgorithm = collisionAlgorithm.get();

			PhysicsTransform shapeWorldTransform = body1->getPhysicsTransform() * localizedShape->transform;
			CollisionObjectWrapper object1(*localizedShape->shape, shapeWorldTransform);
			CollisionObjectWrapper object2(otherShape, body2->getPhysicsTransform());

			collisionAlgorithm->processCollisionAlgorithm(object1, object2, false);

			const ManifoldResult &algorithmManifoldResult = constCollisionAlgorithm->getManifoldResult();
			addContactPointsToManifold(algorithmManifoldResult, constCollisionAlgorithm->isObjectSwapped());
		}
	}

	void CompoundCollisionAlgorithm::addContactPointsToManifold(const ManifoldResult &manifoldResult, bool manifoldSwapped)
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
						false);

			}else
			{
				getManifoldResult().addContactPoint(
						manifolContactPoint.getNormalFromObject2(),
						manifolContactPoint.getPointOnObject1(),
						manifolContactPoint.getPointOnObject2(),
						manifolContactPoint.getLocalPointOnObject1(),
						manifolContactPoint.getLocalPointOnObject2(),
						manifolContactPoint.getDepth(),
						true);
			}
		}
	}

	CollisionAlgorithm *CompoundCollisionAlgorithm::Builder::createCollisionAlgorithm(bool objectSwapped, const ManifoldResult &result, void* memPtr) const
	{
		return new(memPtr) CompoundCollisionAlgorithm(objectSwapped, result);
	}

	CollisionShape3D::ShapeType CompoundCollisionAlgorithm::Builder::getShapeType1() const
	{
		return CollisionShape3D::COMPOUND_SHAPE;
	}

	CollisionShape3D::ShapeType CompoundCollisionAlgorithm::Builder::getShapeType2() const
	{
		return CollisionShape3D::ANY_TYPE;
	}

	unsigned int CompoundCollisionAlgorithm::Builder::getAlgorithmSize() const
	{
		return sizeof(CompoundCollisionAlgorithm);
	}

}
