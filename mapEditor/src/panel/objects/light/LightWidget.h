#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <UrchinAggregation.h>

#include <controller/objects/ObjectController.h>

namespace urchin {

    class LightWidget final : public QWidget {
        Q_OBJECT

        public:
            LightWidget();

            void load(const ObjectEntity&, ObjectController&);

        private:
            void setupGeneralPropertiesBox(QVBoxLayout*);
            void setupSpecificSunLightBox(QVBoxLayout*);
            void setupSpecificOmnidirectionalLightBox(QVBoxLayout*);
            void setupSpecificSpotLightBox(QVBoxLayout*);

            void setupLightDataFrom();
            void setupSunLightDataFrom(const SunLight*) const;
            void setupOmnidirectionalLightDataFrom(const OmnidirectionalLight*) const;
            void setupSpotLightDataFrom(const SpotLight*) const;

            const ObjectEntity* objectEntity;
            ObjectController* objectController;

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
            void updateLightGeneralProperties() const;
            void updateLightSpecificProperties() const;
            
    };

}
