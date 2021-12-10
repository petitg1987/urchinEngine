#include <controller/sky/SkyController.h>

namespace urchin {
    SkyController::SkyController() :
            AbstractController() {

    }

    const SkyEntity& SkyController::getSkyEntity() const {
        return getMapHandler()->getMap().getSkyEntity();
    }

    const SkyEntity& SkyController::updateSkyEntity(const std::vector<std::string>& skyboxFilenames, float offsetY) {
        std::unique_ptr<Skybox> updatedSkybox(nullptr);
        if (!isSkyboxFilenamesAllEmpty(skyboxFilenames)) {
            updatedSkybox = std::make_unique<Skybox>(skyboxFilenames);
            updatedSkybox->setOffsetY(offsetY);
        }
        getMapHandler()->getMap().updateSkyEntity(std::move(updatedSkybox));

        markModified();
        return getMapHandler()->getMap().getSkyEntity();
    }

    bool SkyController::isSkyboxFilenamesAllEmpty(const std::vector<std::string>& skyboxFilenames) const {
        return std::ranges::all_of(skyboxFilenames, [](const auto& skyboxFilename){return skyboxFilename.empty();});
    }
}
