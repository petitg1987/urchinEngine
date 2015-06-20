#include "LightControllerWidget.h"
#include "support/GroupBoxStyleHelper.h"
#include "support/SpinBoxStyleHelper.h"
#include "scene/controller/lights/dialog/NewLightDialog.h"

namespace urchin
{

	LightControllerWidget::LightControllerWidget() :
			lightController(nullptr),
			disableLightEvent(false)
	{
		lightTableView = new LightTableView(this);
		lightTableView->addObserver(this, LightTableView::SELECTION_CHANGED);
		lightTableView->setGeometry(QRect(0, 0, 355, 220));

		addLightButton = new QPushButton(this);
		addLightButton->setText("New Light");
		addLightButton->setGeometry(QRect(0, 221, 85, 22));
		connect(addLightButton, SIGNAL(clicked()), this, SLOT(showAddLightDialog()));

		removeLightButton = new QPushButton(this);
		removeLightButton->setText("Remove Light");
		removeLightButton->setGeometry(QRect(86, 221, 85, 22));
		removeLightButton->setEnabled(false);
		connect(removeLightButton, SIGNAL(clicked()), this, SLOT(removeSelectedLight()));

		lightGroupBox = new QGroupBox(this);
		lightGroupBox->setTitle("Light");
		lightGroupBox->setGeometry(QRect(0, 250, 355, 525));
		GroupBoxStyleHelper::applyNormalStyle(lightGroupBox);
		lightGroupBox->hide();

		setupGeneralPropertiesBox(lightGroupBox);
		setupSpecificPropertiesBox(lightGroupBox);
	}

	LightControllerWidget::~LightControllerWidget()
	{

	}

	void LightControllerWidget::setupGeneralPropertiesBox(QWidget *lightBox)
	{
		QGroupBox *generalPropertiesGroupBox = new QGroupBox(lightBox);
		generalPropertiesGroupBox->setTitle("General Properties");
		generalPropertiesGroupBox->setGeometry(QRect(5, 15, 345, 65));
		GroupBoxStyleHelper::applyNormalStyle(generalPropertiesGroupBox);

		QLabel *ambientLabel= new QLabel(generalPropertiesGroupBox);
		ambientLabel->setText("Ambient:");
		ambientLabel->setGeometry(QRect(5, 15, 55, 22));

		ambientR = new QDoubleSpinBox(generalPropertiesGroupBox);
		ambientR->setGeometry(QRect(85, 15, 80, 22));
		SpinBoxStyleHelper::applyNormalStyle(ambientR);
		ambientR->setMinimum(0.0);
		ambientR->setMaximum(1.0);
		connect(ambientR, SIGNAL(valueChanged(double)), this, SLOT(updateLightGeneralProperties()));
		ambientG = new QDoubleSpinBox(generalPropertiesGroupBox);
		ambientG->setGeometry(QRect(165, 15, 80, 22));
		SpinBoxStyleHelper::applyNormalStyle(ambientG);
		ambientG->setMinimum(0.0);
		ambientG->setMaximum(1.0);
		connect(ambientG, SIGNAL(valueChanged(double)), this, SLOT(updateLightGeneralProperties()));
		ambientB = new QDoubleSpinBox(generalPropertiesGroupBox);
		ambientB->setGeometry(QRect(245, 15, 80, 22));
		SpinBoxStyleHelper::applyNormalStyle(ambientB);
		ambientB->setMinimum(0.0);
		ambientB->setMaximum(1.0);
		connect(ambientB, SIGNAL(valueChanged(double)), this, SLOT(updateLightGeneralProperties()));

		produceShadowCheckBox = new QCheckBox(generalPropertiesGroupBox);
		produceShadowCheckBox->setText("Product Shadow");
		produceShadowCheckBox->setGeometry(QRect(5, 40, 110, 22));
		connect(produceShadowCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateLightGeneralProperties()));
	}

	void LightControllerWidget::setupSpecificPropertiesBox(QWidget *lightBox)
	{
		QGroupBox *specificPropertiesGroupBox = new QGroupBox(lightBox);
		specificPropertiesGroupBox->setTitle("Specific Properties");
		specificPropertiesGroupBox->setGeometry(QRect(5, 85, 345, 95));
		GroupBoxStyleHelper::applyNormalStyle(specificPropertiesGroupBox);

		QLabel *lightTypeLabel= new QLabel(specificPropertiesGroupBox);
		lightTypeLabel->setText("Light Type:");
		lightTypeLabel->setGeometry(QRect(5, 15, 55, 22));

		lightType = new QLabel(specificPropertiesGroupBox);
		lightType->setGeometry(QRect(85, 15, 80, 22));

		setupOmnidirectionalLightBox(specificPropertiesGroupBox);
		setupSunLightBox(specificPropertiesGroupBox);
	}

	void LightControllerWidget::setupOmnidirectionalLightBox(QWidget *specificPropertiesGroupBox)
	{
		omnidirectionalLightWidget = new QWidget(specificPropertiesGroupBox);
		omnidirectionalLightWidget->setGeometry(QRect(5, 40, 345, 85));
		omnidirectionalLightWidget->hide();

		QLabel *positionLabel= new QLabel(omnidirectionalLightWidget);
		positionLabel->setText("Position:");
		positionLabel->setGeometry(QRect(0, 0, 55, 22));

		positionX = new QDoubleSpinBox(omnidirectionalLightWidget);
		positionX->setGeometry(QRect(85, 0, 80, 22));
		SpinBoxStyleHelper::applyNormalStyle(positionX);
		connect(positionX, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));
		positionY = new QDoubleSpinBox(omnidirectionalLightWidget);
		positionY->setGeometry(QRect(165, 0, 80, 22));
		SpinBoxStyleHelper::applyNormalStyle(positionY);
		connect(positionY, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));
		positionZ = new QDoubleSpinBox(omnidirectionalLightWidget);
		positionZ->setGeometry(QRect(245, 0, 80, 22));
		SpinBoxStyleHelper::applyNormalStyle(positionZ);
		connect(positionZ, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));

		QLabel *attenuationLabel= new QLabel(omnidirectionalLightWidget);
		attenuationLabel->setText("Expo. Att.:");
		attenuationLabel->setGeometry(QRect(0, 25, 55, 22));

		attenuation = new QDoubleSpinBox(omnidirectionalLightWidget);
		attenuation->setGeometry(QRect(85, 25, 80, 22));
		SpinBoxStyleHelper::applyNormalStyle(attenuation);
		attenuation->setMinimum(0.0);
		attenuation->setSingleStep(0.05);
		connect(attenuation, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));
	}

	void LightControllerWidget::setupSunLightBox(QWidget *specificPropertiesGroupBox)
	{
		sunLightWidget = new QWidget(specificPropertiesGroupBox);
		sunLightWidget->setGeometry(QRect(5, 40, 345, 85));
		sunLightWidget->hide();

		QLabel *directionLabel= new QLabel(sunLightWidget);
		directionLabel->setText("Direction:");
		directionLabel->setGeometry(QRect(0, 0, 55, 22));

		directionX = new QDoubleSpinBox(sunLightWidget);
		directionX->setGeometry(QRect(85, 0, 80, 22));
		SpinBoxStyleHelper::applyNormalStyle(directionX);
		connect(directionX, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));
		directionY = new QDoubleSpinBox(sunLightWidget);
		directionY->setGeometry(QRect(165, 0, 80, 22));
		SpinBoxStyleHelper::applyNormalStyle(directionY);
		connect(directionY, SIGNAL(valueChanged(double)), this, SLOT(updateLightSpecificProperties()));
		directionZ = new QDoubleSpinBox(sunLightWidget);
		directionZ->setGeometry(QRect(245, 0, 80, 22));
		SpinBoxStyleHelper::applyNormalStyle(directionZ);
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
						lightGroupBox->show();
					}else
					{
						removeLightButton->setEnabled(false);
						lightGroupBox->hide();
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
		omnidirectionalLightWidget->show();
		sunLightWidget->hide();

		lightType->setText(OMNIDIRECTIONAL_LIGHT_LABEL);

		this->positionX->setValue(light->getPosition().X);
		this->positionY->setValue(light->getPosition().Y);
		this->positionZ->setValue(light->getPosition().Z);

		this->attenuation->setValue(light->getExponentialAttenuation());
	}

	void LightControllerWidget::setupSunLightDataFrom(const SunLight *light)
	{
		sunLightWidget->show();
		omnidirectionalLightWidget->hide();

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
