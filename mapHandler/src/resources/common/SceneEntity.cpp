#include <resources/common/SceneEntity.h>

namespace urchin {

    void SceneEntity::refresh() {
        const RigidBody* rigidBody = getRigidBody();
        if (rigidBody && rigidBody->isActive()) {
            PhysicsTransform physicsTransform = rigidBody->getTransform();
            moveTo(physicsTransform.getPosition(), physicsTransform.getOrientation());
        }
    }

}
