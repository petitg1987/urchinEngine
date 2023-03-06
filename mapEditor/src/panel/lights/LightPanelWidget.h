#pragma once

#include <string>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>

#include <UrchinCommon.h>
#include <controller/lights/LightController.h>
#include <panel/lights/LightTableView.h>

namespace urchin {

    class LightPanelWidget final : public QWidget, public Observer {
        Q_OBJECT

        public:
            LightPanelWidget();
            ~LightPanelWidget() override = default;

            LightTableView* getLightTableView() const;

            void load(LightController&);
            void unload();

        private:
            void setupGeneralPropertiesBox(QVBoxLayout*);
            void setupSpecificSunLightBox(QVBoxLayout*);
            void setupSpecificOmnidirectionalLightBox(QVBoxLayout*);
            void setupSpecificSpotLightBox(QVBoxLayout*);

            void notify(Observable*, int) override;
            void setupLightDataFrom(const LightEntity*);
            void setupSunLightDataFrom(const SunLight*);
            void setupOmnidirectionalLightDataFrom(const OmnidirectionalLight*);
            void setupSpotLightDataFrom(const SpotLight*);

            LightController* lightController;

            LightTableView* lightTableView;
            QPushButton* addLightButton;
            QPushButton* removeLightButton;

            QGroupBox* generalPropertiesGroupBox;
            QGroupBox* specificSunLightGroupBox;
            QGroupBox* specificOmnidirectionalLightGroupBox;
            QGroupBox* specificSpotLightGroupBox;

            bool disableLightEvent;

            //general properties
            QDoubleSpinBox* colorR;
            QDoubleSpinBox* colorG;
            QDoubleSpinBox* colorB;
            QCheckBox* produceShadowCheckBox;
            QLabel* lightType;

            //specific properties
            QDoubleSpinBox* positionX;
            QDoubleSpinBox* positionY;
            QDoubleSpinBox* positionZ;
            QDoubleSpinBox* attenuation;
            QDoubleSpinBox* directionX;
            QDoubleSpinBox* directionY;
            QDoubleSpinBox* directionZ;
            QDoubleSpinBox* innerAngle;
            QDoubleSpinBox* outerAngle;

        private slots:
            void showAddLightDialog();
            void removeSelectedLight();

            void updateLightGeneralProperties();
            void updateLightSpecificProperties();
    };

}
