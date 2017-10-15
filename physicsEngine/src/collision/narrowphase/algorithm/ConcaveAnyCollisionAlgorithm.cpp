#include "ConcaveAnyCollisionAlgorithm.h"

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
        //TODO implement it
    }

    CollisionAlgorithm *ConcaveAnyCollisionAlgorithm::Builder::createCollisionAlgorithm(bool objectSwapped, const ManifoldResult &result, void* memPtr) const
    {
        return new(memPtr) ConcaveAnyCollisionAlgorithm(objectSwapped, result);
    }

    CollisionShape3D::ShapeType ConcaveAnyCollisionAlgorithm::Builder::getFirstExpectedShapeType() const
    {
        return CollisionShape3D::HEIGHTFIELD_SHAPE; //TODO wrong: don't include all types of concave
    }

    unsigned int ConcaveAnyCollisionAlgorithm::Builder::getAlgorithmSize() const
    {
        return sizeof(ConcaveAnyCollisionAlgorithm);
    }

}
