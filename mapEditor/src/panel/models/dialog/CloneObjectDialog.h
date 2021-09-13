#pragma once

#include <list>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>

#include <UrchinMapHandler.h>
#include <controller/objects/ObjectController.h>

namespace urchin {

    class CloneObjectDialog : public QDialog {
            Q_OBJECT

        public:
            CloneObjectDialog(QWidget*, const ObjectController*);

            std::unique_ptr<SceneModel> moveSceneModel();

        private:
            void setupNameFields(QGridLayout*);

            void updateObjectName();
            int buildSceneModel(int);

            void done(int) override;
            bool isSceneModelExist(const std::string&);

            const ObjectController* modelController;

            QLabel* modelNameLabel;
            QLineEdit* modelNameText;

            std::string modelName;
            std::unique_ptr<SceneModel> sceneModel;
    };

}
