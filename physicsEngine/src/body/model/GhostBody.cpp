#include "body/model/GhostBody.h"
#include "collision/broadphase/SyncVectorPairContainer.h"

namespace urchin {

    GhostBody::GhostBody(const std::string& id, const PhysicsTransform& physicsTransform, const std::shared_ptr<const CollisionShape3D>& shape) :
            AbstractBody(id, Transform<float>(physicsTransform.getPosition(), physicsTransform.getOrientation(), 1.0f), shape),
            pairContainer(new SyncVectorPairContainer()) {
        AbstractBody::setIsStatic(false); //can move and be affected by the physics world: not a static body
        setIsActive(false); //default value: body is not active
    }

    GhostBody::~GhostBody() {
        delete pairContainer;
    }

    GhostBody* GhostBody::upCast(AbstractWorkBody* workBody) {
        return dynamic_cast<GhostBody*>(workBody);
    }

    const GhostBody* GhostBody::upCast(const AbstractWorkBody* workBody) {
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
