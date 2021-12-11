#include <resources/sky/SkyEntity.h>

namespace urchin {
    SkyEntity::SkyEntity(Renderer3d* renderer3d) :
            renderer3d(renderer3d) {

    }

    const Skybox* SkyEntity::getSkybox() const {
        return renderer3d->getSkyContainer().getSkybox();
    }

    void SkyEntity::changeSkybox(std::unique_ptr<Skybox> skybox) {
        renderer3d->getSkyContainer().setSkybox(std::move(skybox));
    }

}
