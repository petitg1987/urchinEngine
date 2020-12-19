#include "SceneEntity.h"

namespace urchin {
    void SceneEntity::refresh() {
        RigidBody* rigidBody = getRigidBody();
        if (rigidBody) {
            if (rigidBody->isActive() || rigidBody->getManuallyMovedAndReset()) {
                moveTo(rigidBody->getTransform().toTransform());
            }
        }
    }
}
