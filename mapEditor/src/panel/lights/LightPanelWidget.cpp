#include <QtWidgets/QHBoxLayout>

#include <panel/lights/LightPanelWidget.h>
#include <widget/style/GroupBoxStyleHelper.h>
#include <widget/style/SpinBoxStyleHelper.h>
#include <widget/style/ButtonStyleHelper.h>
#include <panel/lights/dialog/NewLightDialog.h>

namespace urchin {

    LightPanelWidget::LightPanelWidget() :
            lightController(nullptr),
            generalPropertiesGroupBox(nullptr),
            specificOmnidirectionalLightGroupBox(nullptr),
            specificSunLightGroupBox(nullptr),
            disableLightEvent(false),
            colorR(nullptr),
            colorG(nullptr),
            colorB(nullptr),
            produceShadowCheckBox(nullptr),
            lightType(nullptr),
            positionX(nullptr),
            positionY(nullptr),
            positionZ(nullptr),
            attenuation(nullptr),
            directionX(nullptr),
            directionY(nullptr),
            directionZ(nullptr) {
        auto* mainLayout = new QVBoxLayout(this);
        mainLayout->setAlignment(Qt::AlignTop);
        mainLayout->setContentsMargins(1, 1, 1, 1);

        lightTableView = new LightTableView();
        mainLayout->addWidget(lightTableView);
        lightTableView->addObserver(this, LightTableView::LIGHT_SELECTION_CHANGED);
        lightTableView->setFixedHeight(320);

        auto* buttonsLayout = new QHBoxLayout();
        mainLayout->addLayout(buttonsLayout);
        buttonsLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        addLightButton = new QPushButton("New");
        buttonsLayout->addWidget(addLightButton);
        ButtonStyleHelper::applyNormalStyle(addLightButton);
        connect(addLightButton, SIGNAL(clicked()), this, SLOT(showAddLightDialog()));

        removeLightButton = new QPushButton("Remove");
        buttonsLayout->addWidget(removeLightButton);
        ButtonStyleHelper::applyNormalStyle(removeLightButton);
        removeLightButton->setEnabled(false);
        connect(removeLightButton, SIGNAL(clicked()), this, SLOT(removeSelectedLight()));

        setupGeneralPropertiesBox(mainLayout);
        setupSpecificOmnidirectionalLightBox(mainLayout);
        setupSpecificSunLightBox(mainLayout);
    }

    void LightPanelWidget::setupGeneralPropertiesBox(QVBoxLayout* mainLayout) {
        generalPropertiesGroupBox = new QGroupBox("General Properties");
        mainLayout->addWidget(generalPropertiesGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(generalPropertiesGroupBox);
        generalPropertiesGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        generalPropertiesGroupBox->hide();

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

        produceShadowCheckBox = new QCheckBox("Product Shadow");
        generalPropertiesLayout->addWidget(produceShadowCheckBox, 1, 0, 1, 2);
        connect(produceShadowCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateLightGeneralProperties()));

        auto* lightTypeLabel= new QLabel("Light Type:");
        generalPropertiesLayout->addWidget(lightTypeLabel, 2, 0);

        lightType = new QLabel();
        generalPropertiesLayout->addWidget(lightType, 2, 1);
    }

    void LightPanelWidget::setupSpecificOmnidirectionalLightBox(QVBoxLayout* mainLayout) {
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
        positionX = new QDoubleSpinBox();
        positionLayout->addWidget(positionX);
        SpinBoxStyleHelper::applyDefaultStyleOn(positionX);
        connect(positionX, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));
        positionY = new QDoubleSpinBox();
        positionLayout->addWidget(positionY);
        SpinBoxStyleHelper::applyDefaultStyleOn(positionY);
        connect(positionY, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));
        positionZ = new QDoubleSpinBox();
        positionLayout->addWidget(positionZ);
        SpinBoxStyleHelper::applyDefaultStyleOn(positionZ);
        connect(positionZ, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));

        auto* attenuationLabel= new QLabel("Expo. Att.:");
        omniLightLayout->addWidget(attenuationLabel, 1, 0);

        attenuation = new QDoubleSpinBox();
        omniLightLayout->addWidget(attenuation, 1, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(attenuation);
        attenuation->setMinimum(0.01);
        attenuation->setSingleStep(0.05);
        connect(attenuation, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));
    }

    void LightPanelWidget::setupSpecificSunLightBox(QVBoxLayout* mainLayout) {
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
        directionX = new QDoubleSpinBox();
        directionLayout->addWidget(directionX);
        SpinBoxStyleHelper::applyDefaultStyleOn(directionX);
        connect(directionX, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));
        directionY = new QDoubleSpinBox();
        directionLayout->addWidget(directionY);
        SpinBoxStyleHelper::applyDefaultStyleOn(directionY);
        connect(directionY, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));
        directionZ = new QDoubleSpinBox();
        directionLayout->addWidget(directionZ);
        SpinBoxStyleHelper::applyDefaultStyleOn(directionZ);
        connect(directionZ, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));
    }

    LightTableView* LightPanelWidget::getLightTableView() const {
        return lightTableView;
    }

    void LightPanelWidget::load(LightController& lightController) {
        this->lightController = &lightController;

        std::list<const LightEntity*> lightEntities = this->lightController->getLightEntities();
        for (auto& lightEntity : lightEntities) {
            lightTableView->addLight(*lightEntity);
        }
    }

    void LightPanelWidget::unload() {
        lightTableView->removeAllLights();

        lightController = nullptr;
    }

    void LightPanelWidget::notify(Observable* observable, int notificationType) {
        if (const auto* lightTableView = dynamic_cast<LightTableView*>(observable)) {
            if (notificationType == LightTableView::LIGHT_SELECTION_CHANGED) {
                if (lightTableView->hasLightEntitySelected()) {
                    const LightEntity* lightEntity = lightTableView->getSelectedLightEntity();
                    setupLightDataFrom(lightEntity);

                    removeLightButton->setEnabled(true);
                    generalPropertiesGroupBox->show();
                } else {
                    removeLightButton->setEnabled(false);
                    generalPropertiesGroupBox->hide();
                    specificSunLightGroupBox->hide();
                    specificOmnidirectionalLightGroupBox->hide();
                }
            }
        }
    }

    void LightPanelWidget::setupLightDataFrom(const LightEntity* lightEntity) {
        disableLightEvent = true;
        const Light* light = lightEntity->getLight();

        this->colorR->setValue(light->getLightColor().X);
        this->colorG->setValue(light->getLightColor().Y);
        this->colorB->setValue(light->getLightColor().Z);

        this->produceShadowCheckBox->setChecked(light->isProduceShadow());

        if (light->getLightType() == Light::LightType::OMNIDIRECTIONAL) {
            setupOmnidirectionalLightDataFrom(static_cast<const OmnidirectionalLight*>(light));
            this->produceShadowCheckBox->setDisabled(true);
        } else if (light->getLightType() == Light::LightType::SUN) {
            setupSunLightDataFrom(static_cast<const SunLight*>(light));
            this->produceShadowCheckBox->setDisabled(false);
        } else {
            throw std::invalid_argument("Impossible to setup specific light data for light of type: " + std::to_string((int)light->getLightType()));
        }

        disableLightEvent = false;
    }

    void LightPanelWidget::setupOmnidirectionalLightDataFrom(const OmnidirectionalLight* light) {
        specificOmnidirectionalLightGroupBox->show();
        specificSunLightGroupBox->hide();

        lightType->setText(NewLightDialog::OMNIDIRECTIONAL_LIGHT_LABEL);

        this->positionX->setValue(light->getPosition().X);
        this->positionY->setValue(light->getPosition().Y);
        this->positionZ->setValue(light->getPosition().Z);

        this->attenuation->setValue(light->getExponentialAttenuation());
    }

    void LightPanelWidget::setupSunLightDataFrom(const SunLight* light) {
        specificSunLightGroupBox->show();
        specificOmnidirectionalLightGroupBox->hide();

        lightType->setText(NewLightDialog::SUN_LIGHT_LABEL);

        this->directionX->setValue(light->getDirections()[0].X);
        this->directionY->setValue(light->getDirections()[0].Y);
        this->directionZ->setValue(light->getDirections()[0].Z);
    }

    void LightPanelWidget::showAddLightDialog() {
        NewLightDialog newLightEntityDialog(this, lightController);
        newLightEntityDialog.exec();

        if (newLightEntityDialog.result() == QDialog::Accepted) {
            std::unique_ptr<LightEntity> lightEntity = newLightEntityDialog.moveLightEntity();
            const LightEntity* lightEntityPtr = lightEntity.get();
            lightController->addLightEntity(std::move(lightEntity));
            lightController->moveLightInFrontOfCamera(*lightEntityPtr, false);

            int row = lightTableView->addLight(*lightEntityPtr);
            lightTableView->selectRow(row);
        }
    }

    void LightPanelWidget::removeSelectedLight() {
        if (lightTableView->hasLightEntitySelected()) {
            const LightEntity& lightEntity = *lightTableView->getSelectedLightEntity();
            lightController->removeLightEntity(lightEntity);

            lightTableView->removeSelectedLight();
        }
    }

    void LightPanelWidget::updateLightGeneralProperties() {
        if (!disableLightEvent) {
            const LightEntity& lightEntity = *lightTableView->getSelectedLightEntity();

            Point3 lightColor((float)colorR->value(), (float)colorG->value(), (float)colorB->value());
            bool produceShadow = produceShadowCheckBox->isChecked();

            lightController->updateLightGeneralProperties(lightEntity, lightColor, produceShadow);
        }
    }

    void LightPanelWidget::updateLightSpecificProperties() {
        if (!disableLightEvent) {
            const LightEntity& lightEntity = *lightTableView->getSelectedLightEntity();
            const Light* light = lightEntity.getLight();

            if (light->getLightType() == Light::LightType::SUN) {
                Vector3 direction((float)directionX->value(), (float)directionY->value(), (float)directionZ->value());
                lightController->updateSunLightProperties(lightEntity, direction);
            } else if (light->getLightType() == Light::LightType::OMNIDIRECTIONAL) {
                Point3 position((float)positionX->value(), (float)positionY->value(), (float)positionZ->value());
                lightController->updateOmnidirectionalLightProperties(lightEntity, (float)attenuation->value(), position);
            } else {
                throw std::invalid_argument("Unknown light type to update specific properties: " + std::to_string((int)light->getLightType()));
            }
        }
    }

}
