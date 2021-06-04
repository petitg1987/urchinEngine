#pragma once

#include <string>

#include <body/model/AbstractBody.h>

namespace urchin {

    /**
    * A ghost body don't act with the physics world but it's able to known which bodies are in collision with it.
    */
    class GhostBody : public AbstractBody {
        public:
            GhostBody(const std::string&, const PhysicsTransform&, const std::shared_ptr<const CollisionShape3D>&);
            ~GhostBody() override;

            static GhostBody* upCast(AbstractBody*);
            static const GhostBody* upCast(const AbstractBody*);

            PairContainer* getPairContainer() const override;

            bool isGhostBody() const override;

        private:
            PairContainer* pairContainer;

    };

}
