#include <body/model/GhostBody.h>
#include <collision/broadphase/SyncVectorPairContainer.h>

namespace urchin {

    GhostBody::GhostBody(const std::string& id, const PhysicsTransform& physicsTransform, std::unique_ptr<const CollisionShape3D> shape) :
            AbstractBody(id, physicsTransform, std::move(shape)),
            pairContainer(new SyncVectorPairContainer()) {
        setIsStatic(false); //can move and be affected by the physics world: not a static body
        setIsActive(false); //default value: body is not active
    }

    GhostBody::~GhostBody() {
        delete pairContainer;
    }

    GhostBody* GhostBody::upCast(AbstractBody* workBody) {
        return dynamic_cast<GhostBody*>(workBody);
    }

    const GhostBody* GhostBody::upCast(const AbstractBody* workBody) {
        return dynamic_cast<const GhostBody*>(workBody);
    }

    /**
     * @return Pair container used to collect colliding pairs on ghost body
     */
    PairContainer* GhostBody::getPairContainer() const {
        return pairContainer;
    }

    bool GhostBody::isGhostBody() const {
        return true;
    }

}
