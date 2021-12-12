#include <resources/sky/SkyEntity.h>

namespace urchin {
    SkyEntity::SkyEntity() :
            renderer3d(nullptr) {

    }

    SkyEntity::~SkyEntity() {
        if (skybox) {
            renderer3d->getSkyContainer().setSkybox(nullptr);
        }
    }

    void SkyEntity::setup(Renderer3d* renderer3d) {
        if (!renderer3d) {
            throw std::invalid_argument("Cannot specify a null renderer 3d for a sky entity.");
        }

        this->renderer3d = renderer3d;

        renderer3d->getSkyContainer().setSkybox(skybox);
    }

    const Skybox* SkyEntity::getSkybox() const {
        return skybox.get();
    }

    void SkyEntity::setSkybox(std::unique_ptr<Skybox> skybox) {
        this->skybox = std::move(skybox);

        if (renderer3d) {
            renderer3d->getSkyContainer().setSkybox(this->skybox);
        }
    }

}
