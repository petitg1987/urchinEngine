#include <panel/objects/light/LightWidget.h>
#include <widget/style/GroupBoxStyleHelper.h>
#include <widget/style/SpinBoxStyleHelper.h>
#include <panel/objects/dialog/ChangeLightTypeDialog.h>

namespace urchin {

    LightWidget::LightWidget() :
            objectEntity(nullptr),
            objectController(nullptr),
            specificSunLightGroupBox(nullptr),
            specificOmnidirectionalLightGroupBox(nullptr),
            specificSpotLightGroupBox(nullptr),
            disableLightEvent(false),
            colorR(nullptr),
            colorG(nullptr),
            colorB(nullptr),
            enablePbrCheckBox(nullptr),
            produceShadowCheckBox(nullptr),
            sunDirectionX(nullptr),
            sunDirectionY(nullptr),
            sunDirectionZ(nullptr),
            omniPositionX(nullptr),
            omniPositionY(nullptr),
            omniPositionZ(nullptr),
            omniAttenuation(nullptr),
            spotDirectionX(nullptr),
            spotDirectionY(nullptr),
            spotDirectionZ(nullptr),
            spotPositionX(nullptr),
            spotPositionY(nullptr),
            spotPositionZ(nullptr),
            spotAttenuation(nullptr),
            spotInnerAngle(nullptr),
            spotOuterAngle(nullptr) {
        setContentsMargins(0, 0, 0, 0);

        auto* mainLayout = new QVBoxLayout(this);
        mainLayout->setAlignment(Qt::AlignTop);
        mainLayout->setContentsMargins(1, 1, 1, 1);

        setupGeneralPropertiesBox(mainLayout);
        setupSpecificSunLightBox(mainLayout);
        setupSpecificOmnidirectionalLightBox(mainLayout);
        setupSpecificSpotLightBox(mainLayout);
    }

    void LightWidget::load(const ObjectEntity& objectEntity, ObjectController& objectController) {
        this->objectEntity = &objectEntity;
        this->objectController = &objectController;

        if (objectEntity.getLight()) {
            setupLightDataFrom();
        }
    }

    void LightWidget::setupGeneralPropertiesBox(QVBoxLayout* mainLayout) {
        auto* generalPropertiesGroupBox = new QGroupBox("General Properties");
        mainLayout->addWidget(generalPropertiesGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(generalPropertiesGroupBox);
        generalPropertiesGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        auto* generalPropertiesLayout = new QGridLayout(generalPropertiesGroupBox);

        auto* ambientLabel= new QLabel("Color:");
        generalPropertiesLayout->addWidget(ambientLabel, 0, 0);

        auto* colorLayout = new QHBoxLayout();
        generalPropertiesLayout->addLayout(colorLayout, 0, 1);
        colorR = new QDoubleSpinBox();
        colorLayout->addWidget(colorR);
        SpinBoxStyleHelper::applyDefaultStyleOn(colorR);
        colorR->setMinimum(0.0);
        colorR->setMaximum(1.0);
        connect(colorR, SIGNAL(valueChanged(double)), this, SLOT(updateLightGeneralProperties()));
        colorG = new QDoubleSpinBox();
        colorLayout->addWidget(colorG);
        SpinBoxStyleHelper::applyDefaultStyleOn(colorG);
        colorG->setMinimum(0.0);
        colorG->setMaximum(1.0);
        connect(colorG, SIGNAL(valueChanged(double)), this, SLOT(updateLightGeneralProperties()));
        colorB = new QDoubleSpinBox();
        colorLayout->addWidget(colorB);
        SpinBoxStyleHelper::applyDefaultStyleOn(colorB);
        colorB->setMinimum(0.0);
        colorB->setMaximum(1.0);
        connect(colorB, SIGNAL(valueChanged(double)), this, SLOT(updateLightGeneralProperties()));

        enablePbrCheckBox = new QCheckBox("Enable PBR");
        generalPropertiesLayout->addWidget(enablePbrCheckBox, 1, 0, 1, 2);
        connect(enablePbrCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateLightGeneralProperties()));

        produceShadowCheckBox = new QCheckBox("Product Shadow");
        generalPropertiesLayout->addWidget(produceShadowCheckBox, 2, 0, 1, 2);
        connect(produceShadowCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateLightGeneralProperties()));
    }

    void LightWidget::setupSpecificSunLightBox(QVBoxLayout* mainLayout) {
        specificSunLightGroupBox = new QGroupBox("Sun Light");
        mainLayout->addWidget(specificSunLightGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(specificSunLightGroupBox);
        specificSunLightGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        specificSunLightGroupBox->hide();

        auto* sunLightLayout = new QGridLayout(specificSunLightGroupBox);

        auto* directionLabel= new QLabel("Direction:");
        sunLightLayout->addWidget(directionLabel, 0, 0);

        auto* directionLayout = new QHBoxLayout();
        sunLightLayout->addLayout(directionLayout, 0, 1);
        sunDirectionX = new QDoubleSpinBox();
        directionLayout->addWidget(sunDirectionX);
        SpinBoxStyleHelper::applyDefaultStyleOn(sunDirectionX);
        connect(sunDirectionX, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));
        sunDirectionY = new QDoubleSpinBox();
        directionLayout->addWidget(sunDirectionY);
        SpinBoxStyleHelper::applyDefaultStyleOn(sunDirectionY);
        connect(sunDirectionY, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));
        sunDirectionZ = new QDoubleSpinBox();
        directionLayout->addWidget(sunDirectionZ);
        SpinBoxStyleHelper::applyDefaultStyleOn(sunDirectionZ);
        connect(sunDirectionZ, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));
    }

    void LightWidget::setupSpecificOmnidirectionalLightBox(QVBoxLayout* mainLayout) {
        specificOmnidirectionalLightGroupBox = new QGroupBox("Omnidirectional Light");
        mainLayout->addWidget(specificOmnidirectionalLightGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(specificOmnidirectionalLightGroupBox);
        specificOmnidirectionalLightGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        specificOmnidirectionalLightGroupBox->hide();

        auto* omniLightLayout = new QGridLayout(specificOmnidirectionalLightGroupBox);

        auto* positionLabel= new QLabel("Position:");
        omniLightLayout->addWidget(positionLabel, 0, 0);

        auto* positionLayout = new QHBoxLayout();
        omniLightLayout->addLayout(positionLayout, 0, 1);
        omniPositionX = new QDoubleSpinBox();
        positionLayout->addWidget(omniPositionX);
        SpinBoxStyleHelper::applyDefaultStyleOn(omniPositionX);
        connect(omniPositionX, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));
        omniPositionY = new QDoubleSpinBox();
        positionLayout->addWidget(omniPositionY);
        SpinBoxStyleHelper::applyDefaultStyleOn(omniPositionY);
        connect(omniPositionY, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));
        omniPositionZ = new QDoubleSpinBox();
        positionLayout->addWidget(omniPositionZ);
        SpinBoxStyleHelper::applyDefaultStyleOn(omniPositionZ);
        connect(omniPositionZ, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));

        auto* attenuationLabel= new QLabel("Expo. Att.:");
        omniLightLayout->addWidget(attenuationLabel, 1, 0);

        omniAttenuation = new QDoubleSpinBox();
        omniLightLayout->addWidget(omniAttenuation, 1, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(omniAttenuation);
        omniAttenuation->setMinimum(0.01);
        omniAttenuation->setSingleStep(0.05);
        connect(omniAttenuation, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));
    }

    void LightWidget::setupSpecificSpotLightBox(QVBoxLayout* mainLayout) {
        specificSpotLightGroupBox = new QGroupBox("Spot Light");
        mainLayout->addWidget(specificSpotLightGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(specificSpotLightGroupBox);
        specificSpotLightGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        specificSpotLightGroupBox->hide();

        auto* spotLightLayout = new QGridLayout(specificSpotLightGroupBox);

        auto* positionLabel= new QLabel("Position:");
        spotLightLayout->addWidget(positionLabel, 0, 0);

        auto* positionLayout = new QHBoxLayout();
        spotLightLayout->addLayout(positionLayout, 0, 1);
        spotPositionX = new QDoubleSpinBox();
        positionLayout->addWidget(spotPositionX);
        SpinBoxStyleHelper::applyDefaultStyleOn(spotPositionX);
        connect(spotPositionX, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));
        spotPositionY = new QDoubleSpinBox();
        positionLayout->addWidget(spotPositionY);
        SpinBoxStyleHelper::applyDefaultStyleOn(spotPositionY);
        connect(spotPositionY, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));
        spotPositionZ = new QDoubleSpinBox();
        positionLayout->addWidget(spotPositionZ);
        SpinBoxStyleHelper::applyDefaultStyleOn(spotPositionZ);
        connect(spotPositionZ, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));

        auto* directionLabel= new QLabel("Direction:");
        spotLightLayout->addWidget(directionLabel, 1, 0);

        auto* directionLayout = new QHBoxLayout();
        spotLightLayout->addLayout(directionLayout, 1, 1);
        spotDirectionX = new QDoubleSpinBox();
        directionLayout->addWidget(spotDirectionX);
        SpinBoxStyleHelper::applyDefaultStyleOn(spotDirectionX);
        connect(spotDirectionX, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));
        spotDirectionY = new QDoubleSpinBox();
        directionLayout->addWidget(spotDirectionY);
        SpinBoxStyleHelper::applyDefaultStyleOn(spotDirectionY);
        connect(spotDirectionY, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));
        spotDirectionZ = new QDoubleSpinBox();
        directionLayout->addWidget(spotDirectionZ);
        SpinBoxStyleHelper::applyDefaultStyleOn(spotDirectionZ);
        connect(spotDirectionZ, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));

        auto* innerAngleLabel= new QLabel("Inner angle (°):");
        spotLightLayout->addWidget(innerAngleLabel, 2, 0);

        spotInnerAngle = new QDoubleSpinBox();
        spotLightLayout->addWidget(spotInnerAngle, 2, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(spotInnerAngle);
        spotInnerAngle->setMinimum(1.0);
        spotInnerAngle->setMaximum(SpotLight::MAX_ANGLE_DEGREE);
        spotInnerAngle->setSingleStep(1.0);
        connect(spotInnerAngle, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));

        auto* outerAngleLabel= new QLabel("Outer angle (°):");
        spotLightLayout->addWidget(outerAngleLabel, 3, 0);

        spotOuterAngle = new QDoubleSpinBox();
        spotLightLayout->addWidget(spotOuterAngle, 3, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(spotOuterAngle);
        spotOuterAngle->setMinimum(1.0);
        spotOuterAngle->setMaximum(SpotLight::MAX_ANGLE_DEGREE);
        spotOuterAngle->setSingleStep(1.0);
        connect(spotOuterAngle, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));

        auto* attenuationLabel= new QLabel("Expo. Att.:");
        spotLightLayout->addWidget(attenuationLabel, 4, 0);

        spotAttenuation = new QDoubleSpinBox();
        spotLightLayout->addWidget(spotAttenuation, 4, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(spotAttenuation);
        spotAttenuation->setMinimum(0.01);
        spotAttenuation->setSingleStep(0.05);
        connect(spotAttenuation, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));
    }

    void LightWidget::setupLightDataFrom() {
        disableLightEvent = true;

        const Light* light = objectEntity->getLight();

        this->colorR->setValue(light->getLightColor().X);
        this->colorG->setValue(light->getLightColor().Y);
        this->colorB->setValue(light->getLightColor().Z);

        this->enablePbrCheckBox->setChecked(light->isPbrEnabled());
        this->produceShadowCheckBox->setChecked(light->isProduceShadow());

        if (light->getLightType() == Light::LightType::SUN) {
            setupSunLightDataFrom(static_cast<const SunLight*>(light));
            this->produceShadowCheckBox->setDisabled(false);
        } else if (light->getLightType() == Light::LightType::OMNIDIRECTIONAL) {
            setupOmnidirectionalLightDataFrom(static_cast<const OmnidirectionalLight*>(light));
            this->produceShadowCheckBox->setDisabled(true);
        } else if (light->getLightType() == Light::LightType::SPOT) {
            setupSpotLightDataFrom(static_cast<const SpotLight*>(light));
            this->produceShadowCheckBox->setDisabled(true);
        } else {
            throw std::invalid_argument("Impossible to setup specific light data for light of type: " + std::to_string((int)light->getLightType()));
        }

        disableLightEvent = false;
    }

    void LightWidget::setupSunLightDataFrom(const SunLight* light) const {
        specificSunLightGroupBox->show();
        specificOmnidirectionalLightGroupBox->hide();
        specificSpotLightGroupBox->hide();

        this->sunDirectionX->setValue(light->getDirections()[0].X);
        this->sunDirectionY->setValue(light->getDirections()[0].Y);
        this->sunDirectionZ->setValue(light->getDirections()[0].Z);
    }

    void LightWidget::setupOmnidirectionalLightDataFrom(const OmnidirectionalLight* light) const {
        specificSunLightGroupBox->hide();
        specificOmnidirectionalLightGroupBox->show();
        specificSpotLightGroupBox->hide();

        this->omniPositionX->setValue(light->getPosition().X);
        this->omniPositionY->setValue(light->getPosition().Y);
        this->omniPositionZ->setValue(light->getPosition().Z);

        this->omniAttenuation->setValue(light->getExponentialAttenuation());
    }

    void LightWidget::setupSpotLightDataFrom(const SpotLight* light) const {
        specificSunLightGroupBox->hide();
        specificOmnidirectionalLightGroupBox->hide();
        specificSpotLightGroupBox->show();

        this->spotPositionX->setValue(light->getPosition().X);
        this->spotPositionY->setValue(light->getPosition().Y);
        this->spotPositionZ->setValue(light->getPosition().Z);

        this->spotDirectionX->setValue(light->getDirections()[0].X);
        this->spotDirectionY->setValue(light->getDirections()[0].Y);
        this->spotDirectionZ->setValue(light->getDirections()[0].Z);

        this->spotInnerAngle->setValue(light->getInnerAngle());
        this->spotOuterAngle->setValue(light->getOuterAngle());

        this->spotAttenuation->setValue(light->getExponentialAttenuation());
    }

    void LightWidget::updateLightGeneralProperties() const {
        if (!disableLightEvent) {
            Point3 lightColor((float)colorR->value(), (float)colorG->value(), (float)colorB->value());
            bool enablePbr = enablePbrCheckBox->isChecked();
            bool produceShadow = produceShadowCheckBox->isChecked();

            objectController->updateLightGeneralProperties(*objectEntity, lightColor, enablePbr, produceShadow);
        }
    }

    void LightWidget::updateLightSpecificProperties() const {
        if (!disableLightEvent) {
            const Light* light = objectEntity->getLight();

            if (light->getLightType() == Light::LightType::SUN) {
                Vector3 direction((float)sunDirectionX->value(), (float)sunDirectionY->value(), (float)sunDirectionZ->value());
                objectController->updateSunLightProperties(*objectEntity, direction);
            } else if (light->getLightType() == Light::LightType::OMNIDIRECTIONAL) {
                Point3 position((float)omniPositionX->value(), (float)omniPositionY->value(), (float)omniPositionZ->value());
                objectController->updateOmnidirectionalLightProperties(*objectEntity, (float)omniAttenuation->value(), position);
            } else if (light->getLightType() == Light::LightType::SPOT) {
                Point3 position((float)spotPositionX->value(), (float)spotPositionY->value(), (float)spotPositionZ->value());
                Vector3 direction((float)spotDirectionX->value(), (float)spotDirectionY->value(), (float)spotDirectionZ->value());
                objectController->updateSpotLightProperties(*objectEntity, (float)spotAttenuation->value(), position, direction, (float)spotInnerAngle->value(), (float)spotOuterAngle->value());
            } else {
                throw std::invalid_argument("Unknown light type to update specific properties: " + std::to_string((int)light->getLightType()));
            }
        }
    }

}
