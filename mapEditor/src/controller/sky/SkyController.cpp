#include "controller/sky/SkyController.h"

namespace urchin {

    const SkyEntity& SkyController::getSkyEntity() const {
        return getMap().getSkyEntity();
    }

    void SkyController::updateSkyEntity(const std::vector<std::string>& skyboxFilenames, float offsetY) {
        auto skyEntity = std::make_unique<SkyEntity>();
        if (!isSkyboxFilenamesAllEmpty(skyboxFilenames)) {
            auto updatedSkybox = std::make_unique<Skybox>(skyboxFilenames);
            updatedSkybox->setOffsetY(offsetY);
            skyEntity->setSkybox(std::move(updatedSkybox));
        }
        getMap().setSkyEntity(std::move(skyEntity));

        markModified();
    }

    bool SkyController::isSkyboxFilenamesAllEmpty(const std::vector<std::string>& skyboxFilenames) const {
        return std::ranges::all_of(skyboxFilenames, [](const auto& skyboxFilename){return skyboxFilename.empty();});
    }
}
