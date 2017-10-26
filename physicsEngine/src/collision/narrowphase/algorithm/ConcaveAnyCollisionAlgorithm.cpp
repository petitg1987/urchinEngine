#include <vector>

#include "ConcaveAnyCollisionAlgorithm.h"
#include "shape/CollisionConcaveShape.h"
#include "shape/CollisionTriangleShape.h"

namespace urchin
{

    ConcaveAnyCollisionAlgorithm::ConcaveAnyCollisionAlgorithm(bool objectSwapped, const ManifoldResult &result) :
            CollisionAlgorithm(objectSwapped, result),
            collisionAlgorithmSelector(new CollisionAlgorithmSelector())
    {

    }

    ConcaveAnyCollisionAlgorithm::~ConcaveAnyCollisionAlgorithm()
    {
        delete collisionAlgorithmSelector;
    }

    void ConcaveAnyCollisionAlgorithm::doProcessCollisionAlgorithm(const CollisionObjectWrapper &object1, const CollisionObjectWrapper &object2)
    {
        const CollisionShape3D &otherShape = object2.getShape();

        AbstractWorkBody *body1 = getManifoldResult().getBody1();
        AbstractWorkBody *body2 = getManifoldResult().getBody2();

        AABBox<float> aabboxLocalToObject1 = object2.getShape().toAABBox(object1.getShapeWorldTransform().inverse() * object2.getShapeWorldTransform());
        const auto &concaveShape = dynamic_cast<const CollisionConcaveShape &>(object1.getShape());

        std::vector<Triangle3D<float>> triangles = concaveShape.findTrianglesInAABBox(aabboxLocalToObject1);
std::cout<<triangles.size()<<std::endl; //TODO too much triangles !
        for(const auto &triangle : triangles)
        {
            CollisionTriangleShape collisionTriangleShape(triangle.getPoints());

            std::shared_ptr<CollisionAlgorithm> collisionAlgorithm = collisionAlgorithmSelector->createCollisionAlgorithm(
                    body1, &collisionTriangleShape, body2, &otherShape);
            const CollisionAlgorithm *const constCollisionAlgorithm = collisionAlgorithm.get();

            CollisionObjectWrapper subObject1(collisionTriangleShape, object1.getShapeWorldTransform());
            CollisionObjectWrapper subObject2(otherShape, object2.getShapeWorldTransform());

            collisionAlgorithm->processCollisionAlgorithm(subObject1, subObject2, false);

            const ManifoldResult &algorithmManifoldResult = constCollisionAlgorithm->getConstManifoldResult();
            addContactPointsToManifold(algorithmManifoldResult, constCollisionAlgorithm->isObjectSwapped());
        }
    }

    void ConcaveAnyCollisionAlgorithm::addContactPointsToManifold(const ManifoldResult &manifoldResult, bool manifoldSwapped)
    {
        for(unsigned int i=0; i<manifoldResult.getNumContactPoints(); ++i)
        {
            const ManifoldContactPoint &manifoldContactPoint = manifoldResult.getManifoldContactPoint(i);
            if(manifoldSwapped)
            {
                getManifoldResult().addContactPoint(
                        (-manifoldContactPoint.getNormalFromObject2()),
                        manifoldContactPoint.getPointOnObject2(),
                        manifoldContactPoint.getPointOnObject1(),
                        manifoldContactPoint.getLocalPointOnObject2(),
                        manifoldContactPoint.getLocalPointOnObject1(),
                        manifoldContactPoint.getDepth(),
                        manifoldContactPoint.isPredictive());
            }else
            {
                getManifoldResult().addContactPoint(
                        manifoldContactPoint.getNormalFromObject2(),
                        manifoldContactPoint.getPointOnObject1(),
                        manifoldContactPoint.getPointOnObject2(),
                        manifoldContactPoint.getLocalPointOnObject1(),
                        manifoldContactPoint.getLocalPointOnObject2(),
                        manifoldContactPoint.getDepth(),
                        manifoldContactPoint.isPredictive());
            }
        }
    }

    CollisionAlgorithm *ConcaveAnyCollisionAlgorithm::Builder::createCollisionAlgorithm(bool objectSwapped, const ManifoldResult &result, void* memPtr) const
    {
        return new(memPtr) ConcaveAnyCollisionAlgorithm(objectSwapped, result);
    }

    std::set<CollisionShape3D::ShapeType> ConcaveAnyCollisionAlgorithm::Builder::getFirstExpectedShapeType() const
    {
        return CollisionShape3D::CONCAVE_SHAPES;
    }

    unsigned int ConcaveAnyCollisionAlgorithm::Builder::getAlgorithmSize() const
    {
        return sizeof(ConcaveAnyCollisionAlgorithm);
    }

}
