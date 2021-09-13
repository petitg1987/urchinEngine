#pragma once

#include <list>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>

#include <UrchinMapHandler.h>
#include <controller/objects/ObjectController.h>

namespace urchin {

    class NewObjectDialog : public QDialog {
        Q_OBJECT

        public:
            NewObjectDialog(QWidget*, const ObjectController*);

            std::unique_ptr<SceneModel> moveSceneModel();

        private:
            void setupNameFields(QGridLayout*);
            void setupMeshFilenameFields(QGridLayout*);

            void updateModelName();
            int buildSceneModel(int);

            void done(int) override;
            bool isSceneModelExist(const std::string&);

            const ObjectController* modelController;

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
