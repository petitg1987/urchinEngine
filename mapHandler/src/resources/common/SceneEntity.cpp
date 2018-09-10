#include "SceneEntity.h"

namespace urchin
{
    void SceneEntity::refresh()
    {
        RigidBody *rigidBody = getRigidBody();
        if(rigidBody!=nullptr)
        {
            if((!rigidBody->isStatic() && rigidBody->isActive()) || (rigidBody->isManuallyMovedAndResetFlag()))
            {
                moveTo(rigidBody->getTransform());
            }
        }
    }
}
