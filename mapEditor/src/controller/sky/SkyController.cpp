#include <controller/sky/SkyController.h>

namespace urchin {
    SkyController::SkyController() :
            AbstractController() {

    }

    const SkyEntity& SkyController::getSkyEntity() const {
        return getMap().getSkyEntity();
    }

    const SkyEntity& SkyController::updateSkyEntity(const std::vector<std::string>& skyboxFilenames, float offsetY) {
        std::unique_ptr<SkyEntity> skyEntity = std::make_unique<SkyEntity>();
        if (!isSkyboxFilenamesAllEmpty(skyboxFilenames)) {
            std::unique_ptr<Skybox> updatedSkybox = std::make_unique<Skybox>(skyboxFilenames);
            updatedSkybox->setOffsetY(offsetY);
            skyEntity->setSkybox(std::move(updatedSkybox));
        }
        getMap().setSkyEntity(std::move(skyEntity));

        markModified();
        return getMap().getSkyEntity();
    }

    bool SkyController::isSkyboxFilenamesAllEmpty(const std::vector<std::string>& skyboxFilenames) const {
        return std::ranges::all_of(skyboxFilenames, [](const auto& skyboxFilename){return skyboxFilename.empty();});
    }
}
