#include <memory>
#include <cassert>

#include "collision/narrowphase/algorithm/ConvexHullConvexHullCollisionAlgorithm.h"
#include "object/CollisionConvexObject3D.h"

namespace urchin
{

	ConvexHullConvexHullCollisionAlgorithm::ConvexHullConvexHullCollisionAlgorithm(bool objectSwapped, const ManifoldResult &result) :
			CollisionAlgorithm(objectSwapped, result)
	{

	}

	ConvexHullConvexHullCollisionAlgorithm::~ConvexHullConvexHullCollisionAlgorithm()
	{

	}

	void ConvexHullConvexHullCollisionAlgorithm::doProcessCollisionAlgorithm(const CollisionObjectWrapper &object1, const CollisionObjectWrapper &object2)
	{
		//transform convex hull shapes
		std::shared_ptr<CollisionConvexObject3D> convexObject1 = object1.getShape().toConvexObject(object1.getShapeWorldTransform());
		std::shared_ptr<CollisionConvexObject3D> convexObject2 = object2.getShape().toConvexObject(object2.getShapeWorldTransform());

		//process GJK and EPA hybrid algorithms
		std::unique_ptr<GJKResult<double>> gjkResultWithoutMargin = gjkAlgorithm.processGJK(*convexObject1.get(), *convexObject2.get(), false);

		if(gjkResultWithoutMargin->isValidResult())
		{
			if(gjkResultWithoutMargin->isCollide())
			{ //collision detected on reduced objects (without margins)
				std::unique_ptr<GJKResult<double>> gjkResultWithMargin = gjkAlgorithm.processGJK(*convexObject1.get(), *convexObject2.get(), true);

				if(gjkResultWithMargin->isValidResult() && gjkResultWithMargin->isCollide())
				{
					std::unique_ptr<EPAResult<double>> epaResult = epaAlgorithm.processEPA(*convexObject1.get(), *convexObject2.get(), *gjkResultWithMargin.get());

					if(epaResult->isValidResult() && epaResult->isCollide())
					{ //should be always true except for problems due to float imprecision
						const Vector3<double> &normalFromObject2 = (-epaResult->getNormal());
						const Point3<double> &pointOnObject2 = epaResult->getContactPointB();
						const float penetrationDepth = -epaResult->getPenetrationDepth();

						addNewContactPoint(normalFromObject2.cast<float>(), pointOnObject2.cast<float>(), (float)penetrationDepth);
					}
				}
			}else
			{ //collision detected on enlarged objects (with margins) OR no collision detected
				const Vector3<double> &vectorBA = gjkResultWithoutMargin->getClosestPointB().vector(gjkResultWithoutMargin->getClosestPointA());
				float vectorBALength = vectorBA.length();
				float sumMargins = convexObject1->getOuterMargin() + convexObject2->getOuterMargin();
				if(sumMargins > vectorBALength - getContactBreakingThreshold())
				{ //collision detected on enlarged objects
					const Vector3<double> &normalFromObject2 = vectorBA.normalize();
					const Point3<double> &pointOnObject2 = gjkResultWithoutMargin->getClosestPointB().translate(normalFromObject2 * (double)convexObject2->getOuterMargin());
					const float penetrationDepth = vectorBALength - sumMargins;

					addNewContactPoint(normalFromObject2.cast<float>(), pointOnObject2.cast<float>(), (float)penetrationDepth);
				}
			}
		}
	}

	CollisionAlgorithm *ConvexHullConvexHullCollisionAlgorithm::Builder::createCollisionAlgorithm(bool objectSwapped, const ManifoldResult &result, void* memPtr) const
	{
		return new(memPtr) ConvexHullConvexHullCollisionAlgorithm(objectSwapped, result);
	}

	CollisionShape3D::ShapeType ConvexHullConvexHullCollisionAlgorithm::Builder::getShapeType1() const
	{
		return CollisionShape3D::CONVEX_HULL_SHAPE;
	}

	CollisionShape3D::ShapeType ConvexHullConvexHullCollisionAlgorithm::Builder::getShapeType2() const
	{
		return CollisionShape3D::CONVEX_HULL_SHAPE;
	}

	unsigned int ConvexHullConvexHullCollisionAlgorithm::Builder::getAlgorithmSize() const
	{
		return sizeof(ConvexHullConvexHullCollisionAlgorithm);
	}

}
