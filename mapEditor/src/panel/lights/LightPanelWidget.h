#pragma once

#include <QtWidgets/QBoxLayout>
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
            void setupSunLightDataFrom(const SunLight*) const;
            void setupOmnidirectionalLightDataFrom(const OmnidirectionalLight*) const;
            void setupSpotLightDataFrom(const SpotLight*) const;

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
            QCheckBox* enablePbrCheckBox;
            QCheckBox* produceShadowCheckBox;
            QLabel* lightType;

            //specific properties
            QDoubleSpinBox* sunDirectionX;
            QDoubleSpinBox* sunDirectionY;
            QDoubleSpinBox* sunDirectionZ;
            QDoubleSpinBox* omniPositionX;
            QDoubleSpinBox* omniPositionY;
            QDoubleSpinBox* omniPositionZ;
            QDoubleSpinBox* omniAttenuation;
            QDoubleSpinBox* spotDirectionX;
            QDoubleSpinBox* spotDirectionY;
            QDoubleSpinBox* spotDirectionZ;
            QDoubleSpinBox* spotPositionX;
            QDoubleSpinBox* spotPositionY;
            QDoubleSpinBox* spotPositionZ;
            QDoubleSpinBox* spotAttenuation;
            QDoubleSpinBox* spotInnerAngle;
            QDoubleSpinBox* spotOuterAngle;

        private slots:
            void showAddLightDialog();
            void removeSelectedLight() const;

            void updateLightGeneralProperties() const;
            void updateLightSpecificProperties() const;
    };

}
