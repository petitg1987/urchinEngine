#ifndef URCHINENGINE_SCENEENTITY_H
#define URCHINENGINE_SCENEENTITY_H

#include "Urchin3dEngine.h"
#include "UrchinPhysicsEngine.h"

namespace urchin
{

    class SceneEntity
    {
        public:
            virtual ~SceneEntity() = default;

            void refresh();

        protected:
            virtual RigidBody *getRigidBody() const = 0;
            virtual void moveTo(const Transform<float> &) = 0;
    };

}

#endif
