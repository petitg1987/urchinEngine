#include <QtWidgets/QHBoxLayout>

#include "LightControllerWidget.h"
#include "support/GroupBoxStyleHelper.h"
#include "support/SpinBoxStyleHelper.h"
#include "support/ButtonStyleHelper.h"
#include "scene/controller/lights/dialog/NewLightDialog.h"

namespace urchin
{

	LightControllerWidget::LightControllerWidget() :
			lightController(nullptr),
			disableLightEvent(false)
	{
		QVBoxLayout *mainLayout = new QVBoxLayout(this);
		mainLayout->setAlignment(Qt::AlignTop);
		mainLayout->setContentsMargins(1, 1, 1, 1);

		lightTableView = new LightTableView();
		mainLayout->addWidget(lightTableView);
		lightTableView->addObserver(this, LightTableView::SELECTION_CHANGED);
		lightTableView->setFixedHeight(220);

		QHBoxLayout *buttonsLayout = new QHBoxLayout();
		mainLayout->addLayout(buttonsLayout);
		buttonsLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

		addLightButton = new QPushButton("New Light");
		buttonsLayout->addWidget(addLightButton);
		ButtonStyleHelper::applyNormalStyle(addLightButton);
		connect(addLightButton, SIGNAL(clicked()), this, SLOT(showAddLightDialog()));

		removeLightButton = new QPushButton("Remove Light");
		buttonsLayout->addWidget(removeLightButton);
		ButtonStyleHelper::applyNormalStyle(removeLightButton);
		removeLightButton->setEnabled(false);
		connect(removeLightButton, SIGNAL(clicked()), this, SLOT(removeSelectedLight()));

		setupGeneralPropertiesBox(mainLayout);
		setupSpecificOmnidirectionalLightBox(mainLayout);
		setupSpecificSunLightBox(mainLayout);
	}

	LightControllerWidget::~LightControllerWidget()
	{

	}

	void LightControllerWidget::setupGeneralPropertiesBox(QVBoxLayout *mainLayout)
	{
		generalPropertiesGroupBox = new QGroupBox("General Properties");
		mainLayout->addWidget(generalPropertiesGroupBox);
		GroupBoxStyleHelper::applyNormalStyle(generalPropertiesGroupBox);
		generalPropertiesGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
		generalPropertiesGroupBox->hide();

		QGridLayout *generalPropertiesLayout = new QGridLayout(generalPropertiesGroupBox);

		QLabel *ambientLabel= new QLabel("Ambient:");
		generalPropertiesLayout->addWidget(ambientLabel, 0, 0);

		QHBoxLayout *ambientLayout = new QHBoxLayout();
		generalPropertiesLayout->addLayout(ambientLayout, 0, 1);
		ambientR = new QDoubleSpinBox();
		ambientLayout->addWidget(ambientR);
		SpinBoxStyleHelper::applyDefaultStyleOn(ambientR);
		ambientR->setMinimum(0.0);
		ambientR->setMaximum(1.0);
		connect(ambientR, SIGNAL(valueChanged(double)), this, SLOT(updateLightGeneralProperties()));
		ambientG = new QDoubleSpinBox();
		ambientLayout->addWidget(ambientG);
		SpinBoxStyleHelper::applyDefaultStyleOn(ambientG);
		ambientG->setMinimum(0.0);
		ambientG->setMaximum(1.0);
		connect(ambientG, SIGNAL(valueChanged(double)), this, SLOT(updateLightGeneralProperties()));
		ambientB = new QDoubleSpinBox();
		ambientLayout->addWidget(ambientB);
		SpinBoxStyleHelper::applyDefaultStyleOn(ambientB);
		ambientB->setMinimum(0.0);
		ambientB->setMaximum(1.0);
		connect(ambientB, SIGNAL(valueChanged(double)), this, SLOT(updateLightGeneralProperties()));

		produceShadowCheckBox = new QCheckBox("Product Shadow");
		generalPropertiesLayout->addWidget(produceShadowCheckBox, 1, 0, 1, 2);
		connect(produceShadowCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateLightGeneralProperties()));

		QLabel *lightTypeLabel= new QLabel("Light Type:");
		generalPropertiesLayout->addWidget(lightTypeLabel, 2, 0);

		lightType = new QLabel();
		generalPropertiesLayout->addWidget(lightType, 2, 1);
	}

	void LightControllerWidget::setupSpecificOmnidirectionalLightBox(QVBoxLayout *mainLayout)
	{
		specificOmnidirectionalLightGroupBox = new QGroupBox("Omnidirectional Light");
		mainLayout->addWidget(specificOmnidirectionalLightGroupBox);
		GroupBoxStyleHelper::applyNormalStyle(specificOmnidirectionalLightGroupBox);
		specificOmnidirectionalLightGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
		specificOmnidirectionalLightGroupBox->hide();

		QGridLayout *omniLightLayout = new QGridLayout(specificOmnidirectionalLightGroupBox);

		QLabel *positionLabel= new QLabel("Position:");
		omniLightLayout->addWidget(positionLabel, 0, 0);

		QHBoxLayout *positionLayout = new QHBoxLayout();
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

		QLabel *attenuationLabel= new QLabel("Expo. Att.:");
		omniLightLayout->addWidget(attenuationLabel, 1, 0);

		attenuation = new QDoubleSpinBox();
		omniLightLayout->addWidget(attenuation, 1, 1);
		SpinBoxStyleHelper::applyDefaultStyleOn(attenuation);
		attenuation->setMinimum(0.0);
		attenuation->setSingleStep(0.05);
		connect(attenuation, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));
	}

	void LightControllerWidget::setupSpecificSunLightBox(QVBoxLayout *mainLayout)
	{
		specificSunLightGroupBox = new QGroupBox("Sun Light");
		mainLayout->addWidget(specificSunLightGroupBox);
		GroupBoxStyleHelper::applyNormalStyle(specificSunLightGroupBox);
		specificSunLightGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
		specificSunLightGroupBox->hide();

		QGridLayout *sunLightLayout = new QGridLayout(specificSunLightGroupBox);

		QLabel *directionLabel= new QLabel("Direction:");
		sunLightLayout->addWidget(directionLabel, 0, 0);

		QHBoxLayout *directionLayout = new QHBoxLayout();
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

	LightTableView *LightControllerWidget::getLightTableView() const
	{
		return lightTableView;
	}

	void LightControllerWidget::load(LightController *lightController)
	{
		this->lightController = lightController;

		std::list<const SceneLight *> sceneLights = lightController->getSceneLights();
		for(std::list<const SceneLight *>::const_iterator it=sceneLights.begin(); it!=sceneLights.end(); ++it)
		{
			lightTableView->addLight((*it));
		}
	}

	void LightControllerWidget::unload()
	{
		lightTableView->removeAllLights();

		lightController = nullptr;
	}

	void LightControllerWidget::notify(Observable *observable, int notificationType)
	{
		if(LightTableView *lightTableView = dynamic_cast<LightTableView *>(observable))
		{
			switch(notificationType)
			{
				case LightTableView::SELECTION_CHANGED:
					if(lightTableView->hasSceneLightSelected())
					{
						const SceneLight *sceneLight = lightTableView->getSelectedSceneLight();
						setupLightDataFrom(sceneLight);

						removeLightButton->setEnabled(true);
						generalPropertiesGroupBox->show();
					}else
					{
						removeLightButton->setEnabled(false);
						generalPropertiesGroupBox->hide();
					}
					break;
			}
		}
	}

	void LightControllerWidget::setupLightDataFrom(const SceneLight *sceneLight)
	{
		disableLightEvent = true;
		const Light *light = sceneLight->getLight();

		this->ambientR->setValue(light->getAmbientColor().X);
		this->ambientG->setValue(light->getAmbientColor().Y);
		this->ambientB->setValue(light->getAmbientColor().Z);

		this->produceShadowCheckBox->setChecked(light->isProduceShadow());

		if(light->getLightType()==Light::LightType::OMNIDIRECTIONAL)
		{
			setupOmnidirectionalLightDataFrom(static_cast<const OmnidirectionalLight *>(light));
			this->produceShadowCheckBox->setDisabled(true);
		}else if(light->getLightType()==Light::LightType::SUN)
		{
			setupSunLightDataFrom(static_cast<const SunLight *>(light));
			this->produceShadowCheckBox->setDisabled(false);
		}else
		{
			throw std::invalid_argument("Impossible to setup specific light data for light of type: " + light->getLightType());
		}

		disableLightEvent = false;
	}

	void LightControllerWidget::setupOmnidirectionalLightDataFrom(const OmnidirectionalLight *light)
	{
		specificOmnidirectionalLightGroupBox->show();
		specificSunLightGroupBox->hide();

		lightType->setText(OMNIDIRECTIONAL_LIGHT_LABEL);

		this->positionX->setValue(light->getPosition().X);
		this->positionY->setValue(light->getPosition().Y);
		this->positionZ->setValue(light->getPosition().Z);

		this->attenuation->setValue(light->getExponentialAttenuation());
	}

	void LightControllerWidget::setupSunLightDataFrom(const SunLight *light)
	{
		specificSunLightGroupBox->show();
		specificOmnidirectionalLightGroupBox->hide();

		lightType->setText(SUN_LIGHT_LABEL);

		this->directionX->setValue(light->getDirections()[0].X);
		this->directionY->setValue(light->getDirections()[0].Y);
		this->directionZ->setValue(light->getDirections()[0].Z);
	}

	void LightControllerWidget::showAddLightDialog()
	{
		NewLightDialog newSceneLightDialog(this, lightController);
		newSceneLightDialog.exec();

		if(newSceneLightDialog.result()==QDialog::Accepted)
		{
			SceneLight *sceneLight = newSceneLightDialog.getSceneLight();
			lightController->addSceneLight(sceneLight);

			lightTableView->addLight(sceneLight);
		}
	}

	void LightControllerWidget::removeSelectedLight()
	{
		if(lightTableView->hasSceneLightSelected())
		{
			const SceneLight *sceneLight = lightTableView->getSelectedSceneLight();
			lightController->removeSceneLight(sceneLight);

			lightTableView->removeSelectedLight();
		}
	}

	void LightControllerWidget::updateLightGeneralProperties()
	{
		if(!disableLightEvent)
		{
			const SceneLight *sceneLight = lightTableView->getSelectedSceneLight();

			Point3<float> ambientColor(ambientR->value(), ambientG->value(), ambientB->value());
			bool produceShadow = produceShadowCheckBox->isChecked();

			lightController->updateSceneLightGeneralProperties(sceneLight, ambientColor, produceShadow);
		}
	}

	void LightControllerWidget::updateLightSpecificProperties()
	{
		if(!disableLightEvent)
		{
			const SceneLight *sceneLight = lightTableView->getSelectedSceneLight();
			const Light *light = sceneLight->getLight();

			if(light->getLightType()==Light::LightType::OMNIDIRECTIONAL)
			{
				float expententialAttenuation = attenuation->value();
				Point3<float> position(positionX->value(), positionY->value(), positionZ->value());

				lightController->updateSceneOmnidirectionalLightProperties(sceneLight, expententialAttenuation, position);
			}else if(light->getLightType()==Light::LightType::SUN)
			{
				Vector3<float> direction(directionX->value(), directionY->value(), directionZ->value());

				lightController->updateSceneSunLightProperties(sceneLight, direction);
			}else
			{
				throw std::invalid_argument("Unknown light type to update specific properties: " + light->getLightType());
			}
		}
	}

}
