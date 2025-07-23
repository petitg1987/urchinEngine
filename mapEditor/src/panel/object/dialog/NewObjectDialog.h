#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <UrchinAggregation.h>

#include "controller/object/ObjectController.h"

namespace urchin {

    class NewObjectDialog final : public QDialog {
        Q_OBJECT

        public:
            NewObjectDialog(QWidget*, const ObjectController*);

            std::unique_ptr<ObjectEntity> moveObjectEntity();

        private:
            void setupNameFields(QGridLayout*);
            void setupMeshFilenameFields(QGridLayout*);

            void updateObjectName();
            int buildObjectEntity(int);

            void done(int) override;
            bool isObjectEntityExist(std::string_view) const;

            const ObjectController* objectController;

            QLabel* objectNameLabel;
            QLineEdit* objectNameText;
            QLabel* meshesFilenameLabel;
            QLineEdit* meshesFilenameText;

            std::string objectName;
            std::string meshesFilename;
            std::unique_ptr<ObjectEntity> objectEntity;
            static QString preferredMeshesPath;

        private slots:
            void showMeshFilenameDialog();
    };

}
