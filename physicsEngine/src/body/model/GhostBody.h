#pragma once

#include <string>
#include <memory>

#include <body/model/AbstractBody.h>

namespace urchin {

    /**
    * A ghost body don't act with the physics world but it's able to known which bodies are in collision with it.
    */
    class GhostBody final : public AbstractBody {
        public:
            GhostBody(std::string, const PhysicsTransform&, std::unique_ptr<const CollisionShape3D>);

            static GhostBody* upCast(AbstractBody*);

            PairContainer* getPairContainer() const override;

        private:
            std::unique_ptr<PairContainer> pairContainer;
    };

}
