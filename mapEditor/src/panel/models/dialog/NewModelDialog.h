#pragma once

#include <list>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>

#include <UrchinMapHandler.h>
#include <controller/models/ModelController.h>

namespace urchin {

    class NewModelDialog : public QDialog {
        Q_OBJECT

        public:
            NewModelDialog(QWidget*, const ModelController*);

            std::unique_ptr<SceneModel> moveSceneModel();

        private:
            void setupNameFields(QGridLayout*);
            void setupMeshFilenameFields(QGridLayout*);

            void updateModelName();
            int buildSceneModel(int);

            void done(int) override;
            bool isSceneModelExist(const std::string&);

            const ModelController* modelController;

            QLabel* modelNameLabel;
            QLineEdit* modelNameText;
            QLabel* meshesFilenameLabel;
            QLineEdit* meshesFilenameText;

            std::string modelName;
            std::string meshesFilename;
            std::unique_ptr<SceneModel> sceneModel;
            static QString preferredMeshesPath;

        private slots:
            void showMeshFilenameDialog();
    };

}
