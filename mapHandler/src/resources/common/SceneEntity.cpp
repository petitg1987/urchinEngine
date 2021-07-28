#include <resources/common/SceneEntity.h>

namespace urchin {

    void SceneEntity::refresh() {
        RigidBody* rigidBody = getRigidBody();
        if (rigidBody) {
            if (rigidBody->isActive() || rigidBody->getManuallyMovedAndReset()) {
                PhysicsTransform physicsTransform = rigidBody->getTransform();
                moveTo(physicsTransform.getPosition(), physicsTransform.getOrientation());
            }
        }
    }

}
