#include <controller/AbstractController.h>

namespace urchin {
    AbstractController::AbstractController() :
            bIsModified(false),
            map(nullptr) {

    }

    void AbstractController::setup(Map& map) {
        this->map = &map;
    }

    bool AbstractController::isModified() const {
        return bIsModified;
    }

    void AbstractController::resetModified() {
        bIsModified = false;
    }

    void AbstractController::markModified() {
        bIsModified = true;

        notifyObservers(this, AbstractController::CHANGES_DONE);
    }

    Map& AbstractController::getMap() const {
        return *map;
    }
}
