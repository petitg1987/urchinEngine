#pragma once

#include <QtWidgets/QWidget>

#include <controller/objects/ObjectController.h>

namespace urchin {

    class SoundWidget final : public QWidget {
        Q_OBJECT

        public:
            SoundWidget();

            void load(const ObjectEntity&, ObjectController&);

        private:
            const ObjectEntity* objectEntity;
            ObjectController* objectController;
    };

}
