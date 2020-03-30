#include "SkyController.h"

namespace urchin
{
    SkyController::SkyController(MapHandler *mapHandler) :
            bIsModified(false),
            mapHandler(mapHandler)
    {

    }

    bool SkyController::isModified() const
    {
        return bIsModified;
    }

    void SkyController::markModified()
    {
        bIsModified = true;
    }

    void SkyController::resetModified()
    {
        bIsModified = false;
    }

    const SceneSky *SkyController::getSceneSky() const
    {
        return mapHandler->getMap()->getSceneSky();
    }

    const SceneSky *SkyController::updateSceneSky(const std::vector<std::string> &skyboxFilenames, float offsetY)
    {
        std::unique_ptr<Skybox> updatedSkybox(nullptr);
        if(!isSkyboxFilenamesAllEmpty(skyboxFilenames))
        {
            updatedSkybox = std::make_unique<Skybox>(skyboxFilenames);
            updatedSkybox->setOffsetY(offsetY);
        }
        mapHandler->getMap()->updateSceneSky(std::move(updatedSkybox));

        markModified();
        return mapHandler->getMap()->getSceneSky();
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
