#ifndef URCHINENGINE_PHYSICSCOLLISIONDISPLAYER_H
#define URCHINENGINE_PHYSICSCOLLISIONDISPLAYER_H

#include <vector>
#include "UrchinCommon.h"
#include "Urchin3dEngine.h"
#include "UrchinPhysicsEngine.h"

namespace urchin {

    class CollisionPointDisplayer {
        public:
            CollisionPointDisplayer(PhysicsWorld*, Renderer3d*);
            ~CollisionPointDisplayer();

            void display();
            void clearDisplay();

        private:
            PhysicsWorld* physicsWorld;
            Renderer3d* renderer3d;

            std::vector<GeometryModel*> contactPointModels;
    };

}

#endif
