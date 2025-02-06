#pragma once

#include <QtWidgets/QWidget>

#include <controller/objects/ObjectController.h>

namespace urchin {

    class PhysicsWidget : public QWidget {
        Q_OBJECT

        public:
            PhysicsWidget(const ObjectEntity&, ObjectController&);

        private:
            const ObjectEntity& objectEntity;
            ObjectController& objectController;
    };

}
