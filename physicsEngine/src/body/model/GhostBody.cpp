#include "body/model/GhostBody.h"
#include "collision/broadphase/SyncVectorPairContainer.h"

namespace urchin {

    GhostBody::GhostBody(std::string id, const PhysicsTransform& physicsTransform, std::unique_ptr<const CollisionShape3D> shape) :
            AbstractBody(BodyType::GHOST, std::move(id), physicsTransform, std::move(shape)),
            pairContainer(std::make_unique<SyncVectorPairContainer>()) {
        setIsStatic(false); //can move and be affected by the physics world: not a static body
        setIsActive(false); //default value: body is not active
    }

    GhostBody* GhostBody::upCast(AbstractBody* abstractBody) {
        if (abstractBody->getBodyType() == BodyType::GHOST) {
            return static_cast<GhostBody*>(abstractBody);
        }
        return nullptr;
    }

    /**
     * @return Pair container used to collect colliding pairs on ghost body
     */
    PairContainer* GhostBody::getPairContainer() const {
        return pairContainer.get();
    }

}
