#pragma once

#include <string>
#include <memory>

#include <body/model/AbstractBody.h>

namespace urchin {

    /**
    * A ghost body don't act with the physics world but it's able to known which bodies are in collision with it.
    */
    class GhostBody : public AbstractBody {
        public:
            GhostBody(const std::string&, const PhysicsTransform&, std::unique_ptr<const CollisionShape3D>);
            ~GhostBody() override = default;

            static GhostBody* upCast(AbstractBody*);

            PairContainer* getPairContainer() const override;

            bool isGhostBody() const override;

        private:
            std::unique_ptr<PairContainer> pairContainer;
    };

}
