#ifndef URCHINENGINE_GHOSTBODY_H
#define URCHINENGINE_GHOSTBODY_H

#include <string>

#include "body/model/AbstractBody.h"

namespace urchin {

    /**
    * A ghost body don't act with the physics world but it's able to known which bodies are in collision with it.
    */
    class GhostBody : public AbstractBody {
        public:
            GhostBody(const std::string&, const PhysicsTransform&, const std::shared_ptr<const CollisionShape3D>&); //TODO use Transform instad of PhysicsTrasnform to be aligned with RigidBody ?
            ~GhostBody() override;

            static GhostBody* upCast(AbstractWorkBody*);
            static const GhostBody* upCast(const AbstractWorkBody*);

            PairContainer* getPairContainer() const override;

            bool isGhostBody() const override;

        private:
            PairContainer* pairContainer;

    };

}

#endif
