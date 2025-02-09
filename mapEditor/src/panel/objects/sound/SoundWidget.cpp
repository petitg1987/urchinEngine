#include <panel/objects/sound/SoundWidget.h>

namespace urchin {

    SoundWidget::SoundWidget() :
            objectEntity(nullptr),
            objectController(nullptr) {

    }

    void SoundWidget::load(const ObjectEntity& objectEntity, ObjectController& objectController) {
        this->objectEntity = &objectEntity;
        this->objectController = &objectController;

        if (objectEntity.getSoundComponent()) {
            //TODO setupLightDataFrom(objectEntity.getLight());
        }
    }

}
