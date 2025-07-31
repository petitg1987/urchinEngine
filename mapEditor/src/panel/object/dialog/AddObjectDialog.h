#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <UrchinAggregation.h>

#include "controller/object/ObjectController.h"

namespace urchin {

    class AddObjectDialog final : public QDialog {
        Q_OBJECT

        public:
            AddObjectDialog(QWidget*, const ObjectController*);

            std::unique_ptr<ObjectEntity> moveObjectEntity();

        private:
            void setupNameFields(QGridLayout*);

            void updateObjectName();
            int buildObjectEntity(int);

            void done(int) override;
            bool isObjectEntityExist(std::string_view) const;

            const ObjectController* objectController;

            QLabel* objectNameLabel;
            QLineEdit* objectNameText;

            std::string objectName;
            std::unique_ptr<ObjectEntity> objectEntity;
    };

}
