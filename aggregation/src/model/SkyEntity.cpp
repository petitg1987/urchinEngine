#include <model/SkyEntity.h>

namespace urchin {
    SkyEntity::SkyEntity() :
            renderer3d(nullptr) {
        //default skybox
        std::vector<std::string> skyboxFilenames = {"", "", "", "", "", ""};
        skybox = std::make_unique<Skybox>(skyboxFilenames);
    }

    SkyEntity::~SkyEntity() {
        if (renderer3d && skybox && renderer3d->getSkyContainer().getSkybox() == skybox.get()) {
            renderer3d->getSkyContainer().setSkybox(nullptr);
        }
    }

    void SkyEntity::setup(Renderer3d* renderer3d) {
        this->renderer3d = renderer3d;

        if (renderer3d) {
            renderer3d->getSkyContainer().setSkybox(skybox);
        }
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
