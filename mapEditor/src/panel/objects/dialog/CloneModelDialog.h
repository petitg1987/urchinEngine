#pragma once

#include <list>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>

#include <UrchinMapHandler.h>
#include <controller/objects/ObjectController.h>

namespace urchin {

    class CloneModelDialog : public QDialog { //TODO rename
            Q_OBJECT

        public:
            CloneModelDialog(QWidget*, const ObjectController*);

            std::unique_ptr<ObjectEntity> moveObjectEntity();

        private:
            void setupNameFields(QGridLayout*);

            void updateObjectName();
            int buildObjectEntity(int);

            void done(int) override;
            bool isObjectEntityExist(const std::string&);

            const ObjectController* objectController;

            QLabel* objectNameLabel;
            QLineEdit* objectNameText;

            std::string objectName;
            std::unique_ptr<ObjectEntity> objectEntity;
    };

}
