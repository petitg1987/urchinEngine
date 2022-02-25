#pragma once

#include <list>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>

#include <UrchinAggregation.h>
#include <controller/objects/ObjectController.h>

namespace urchin {

    class CloneObjectDialog : public QDialog {
            Q_OBJECT

        public:
            CloneObjectDialog(QWidget*, const ObjectController*);

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
