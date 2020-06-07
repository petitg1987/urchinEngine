#include "SkyController.h"

namespace urchin
{
    SkyController::SkyController(MapHandler *mapHandler) :
            AbstractController(mapHandler)
    {

    }

    const SceneSky *SkyController::getSceneSky() const
    {
        return getMapHandler()->getMap()->getSceneSky();
    }

    const SceneSky *SkyController::updateSceneSky(const std::vector<std::string> &skyboxFilenames, float offsetY)
    {
        std::unique_ptr<Skybox> updatedSkybox(nullptr);
        if(!isSkyboxFilenamesAllEmpty(skyboxFilenames))
        {
            updatedSkybox = std::make_unique<Skybox>(skyboxFilenames);
            updatedSkybox->setOffsetY(offsetY);
        }
        getMapHandler()->getMap()->updateSceneSky(std::move(updatedSkybox));

        markModified();
        return getMapHandler()->getMap()->getSceneSky();
    }

    bool SkyController::isSkyboxFilenamesAllEmpty(const std::vector<std::string> &skyboxFilenames) const
    {
        for(const auto &skyboxFilename : skyboxFilenames)
        {
            if(!skyboxFilename.empty())
            {
                return false;
            }
        }
        return true;
    }
}
