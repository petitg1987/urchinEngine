#include "AbstractController.h"

namespace urchin
{
    AbstractController::AbstractController() :
        bIsModified(false),
        mapHandler(nullptr)
    {

    }


    void AbstractController::setup(MapHandler *mapHandler)
    {
        this->mapHandler = mapHandler;
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

        notifyObservers(this, AbstractController::CHANGES_DONE);
    }

    MapHandler *AbstractController::getMapHandler() const
    {
        return mapHandler;
    }
}
