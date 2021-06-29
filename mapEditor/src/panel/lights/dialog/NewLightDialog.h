#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>

#include <UrchinMapHandler.h>
#include <controller/lights/LightController.h>

namespace urchin {

    class NewLightDialog : public QDialog {
        Q_OBJECT

        public:
            static constexpr char SUN_LIGHT_LABEL[] = "Sun";
            static constexpr char OMNIDIRECTIONAL_LIGHT_LABEL[] = "Omnidirectional";

            NewLightDialog(QWidget*, const LightController*);

            SceneLight* getSceneLight() const;

        private:
            void setupNameFields(QGridLayout*);
            void setupLightTypeFields(QGridLayout*);

            void updateLightName();
            int buildSceneLight(int);

            void done(int) override;
            bool isSceneLightExist(const std::string&);

            const LightController* lightController;

            QLabel* lightNameLabel;
            QLineEdit* lightNameText;
            QLabel* lightTypeLabel;
            QComboBox* lightTypeComboBox;

            std::string lightName;
            SceneLight* sceneLight;
    };

}
