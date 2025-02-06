#include <panel/objects/physics/PhysicsWidget.h>

namespace urchin {

    PhysicsWidget::PhysicsWidget(const ObjectEntity& objectEntity, ObjectController& objectController) :
            objectEntity(objectEntity),
            objectController(objectController) {
        setContentsMargins(0, 0, 0, 0);

//        auto* mainLayout = new QVBoxLayout(this);
//        mainLayout->setAlignment(Qt::AlignTop);
//        mainLayout->setContentsMargins(1, 1, 1, 1);
//
//        setupGeneralPropertiesBox(mainLayout);
//        setupSpecificSunLightBox(mainLayout);
//        setupSpecificOmnidirectionalLightBox(mainLayout);
//        setupSpecificSpotLightBox(mainLayout);
//
//        if (objectEntity.getLight()) {
//            setupLightDataFrom(objectEntity.getLight());
//        }
    }

}
