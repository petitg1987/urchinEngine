#include <stdexcept>
#include <QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QVariant>

#include "UrchinCommon.h"
#include "NewLightDialog.h"
#include "support/LabelStyleHelper.h"

namespace urchin
{
	NewLightDialog::NewLightDialog(QWidget *parent, const LightController *lightController) :
		QDialog(parent),
		lightController(lightController),
		sceneLight(nullptr)
	{
		this->setWindowTitle("New Light");
		this->resize(530, 130);

		setupNameFields();
		setupLightTypeFields();

		QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
		buttonBox->setGeometry(QRect(30, 90, 475, 22));
		buttonBox->setOrientation(Qt::Horizontal);
		buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

		QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
		QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	}

	void NewLightDialog::setupNameFields()
	{
		lightNameLabel = new QLabel("Light Name:", this);
		lightNameLabel->setGeometry(QRect(10, 20, 110, 22));

		lightNameText = new QLineEdit(this);
		lightNameText->setGeometry(QRect(100, 20, 405, 22));
	}

	void NewLightDialog::setupLightTypeFields()
	{
		lightTypeLabel = new QLabel("Light Type:", this);
		lightTypeLabel->setGeometry(QRect(10, 50, 90, 22));

		lightTypeComboBox = new QComboBox(this);
		lightTypeComboBox->setGeometry(QRect(100, 50, 175, 22));
		lightTypeComboBox->addItem(OMNIDIRECTIONAL_LIGHT_LABEL, QVariant(Light::LightType::OMNIDIRECTIONAL));
		lightTypeComboBox->addItem(SUN_LIGHT_LABEL, QVariant(Light::LightType::SUN));
	}

	void NewLightDialog::updateLightName()
	{
		QString lightName = lightNameText->text();
		if(!lightName.isEmpty())
		{
			this->lightName = lightName.toUtf8().constData();
		}
	}

	int NewLightDialog::buildSceneLight(int result)
	{
		sceneLight = new SceneLight();

		sceneLight->setName(lightName);

		QVariant variant = lightTypeComboBox->currentData();
		Light::LightType lightType = static_cast<Light::LightType>(variant.toInt());

		Light *light;
		if(lightType==Light::OMNIDIRECTIONAL)
		{
			light = new OmnidirectionalLight(Point3<float>(0.0, 0.0, 0.0));
		}else if(lightType==Light::SUN)
		{
			light = new SunLight(Vector3<float>(1.0, -1.0, 0.0));
		}else
		{
			throw std::invalid_argument("Unknown the light type to create a new light: " + lightType);
		}

		sceneLight->setLight(light);

		return result;
	}

	SceneLight *NewLightDialog::getSceneLight() const
	{
		return sceneLight;
	}

	void NewLightDialog::done(int r)
	{
		if(QDialog::Accepted == r)
		{
			bool hasError = false;

			updateLightName();
			LabelStyleHelper::applyNormalStyle(lightNameLabel);

			if(lightName.compare("")==0)
			{
				LabelStyleHelper::applyErrorStyle(lightNameLabel, "Light name is mandatory");
				hasError = true;
			}else if(isSceneLightExist(lightName))
			{
				LabelStyleHelper::applyErrorStyle(lightNameLabel, "Light name is already used");
				hasError = true;
			}

			if(!hasError)
			{
				r = buildSceneLight(r);
				QDialog::done(r);
			}
		}else
		{
			QDialog::done(r);
		}
	}

	bool NewLightDialog::isSceneLightExist(const std::string &name)
	{
		std::list<const SceneLight *> sceneLights = lightController->getSceneLights();
		for(std::list<const SceneLight *>::const_iterator it = sceneLights.begin(); it!=sceneLights.end(); ++it)
		{
			if((*it)->getName().compare(name)==0)
			{
				return true;
			}
		}

		return false;
	}

}
