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
        mapHandler->getMap()->updateSceneSky(skyboxFilenames, offsetY);

        markModified();
        return mapHandler->getMap()->getSceneSky();
    }
}
