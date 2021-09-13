#pragma once

#include <list>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>

#include <UrchinMapHandler.h>
#include <controller/models/ModelController.h>

namespace urchin {

    class CloneModelDialog : public QDialog {
            Q_OBJECT

        public:
            CloneModelDialog(QWidget*, const ModelController*);

            std::unique_ptr<SceneModel> moveSceneModel();

        private:
            void setupNameFields(QGridLayout*);

            void updateModelName();
            int buildSceneModel(int);

            void done(int) override;
            bool isSceneModelExist(const std::string&);

            const ModelController* modelController;

            QLabel* modelNameLabel;
            QLineEdit* modelNameText;

            std::string modelName;
            std::unique_ptr<SceneModel> sceneModel;
    };

}
