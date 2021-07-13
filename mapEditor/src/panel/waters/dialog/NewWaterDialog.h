#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>

#include <UrchinMapHandler.h>
#include <controller/waters/WaterController.h>

namespace urchin {

    class NewWaterDialog : public QDialog {
        Q_OBJECT

        public:
            NewWaterDialog(QWidget*, const WaterController*);

            std::unique_ptr<SceneWater> moveSceneWater();

        private:
            void setupNameFields(QGridLayout*);

            void updateWaterName();
            int buildSceneWater(int);

            void done(int) override;
            bool isSceneWaterExist(const std::string&);

            const WaterController* waterController;

            QLabel* waterNameLabel;
            QLineEdit* waterNameText;

            std::string waterName;
            std::unique_ptr<SceneWater> sceneWater;
    };

}
