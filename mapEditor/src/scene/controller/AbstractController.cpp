#include "AbstractController.h"

namespace urchin
{
    AbstractController::AbstractController(MapHandler *mapHandler) :
        bIsModified(false),
        mapHandler(mapHandler)
    {

    }

    bool AbstractController::isModified() const
    {
        return bIsModified;
    }

    void AbstractController::resetModified()
    {
        bIsModified = false;
    }

    void AbstractController::markModified()
    {
        bIsModified = true;
    }

    MapHandler *AbstractController::getMapHandler() const
    {
        return mapHandler;
    }
}
