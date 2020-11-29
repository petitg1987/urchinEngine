#include <stdexcept>
#include <QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QVariant>

#include "UrchinCommon.h"
#include "NewLightDialog.h"
#include "widget/style/LabelStyleHelper.h"

namespace urchin {
    NewLightDialog::NewLightDialog(QWidget* parent, const LightController* lightController) :
        QDialog(parent),
        lightController(lightController),
        lightNameLabel(nullptr),
        lightNameText(nullptr),
        lightTypeLabel(nullptr),
        lightTypeComboBox(nullptr),
        sceneLight(nullptr) {
        this->setWindowTitle("New Light");
        this->resize(530, 130);
        this->setFixedSize(this->width(),this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupNameFields(mainLayout);
        setupLightTypeFields(mainLayout);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 2, 0, 1, 2, Qt::AlignRight);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void NewLightDialog::setupNameFields(QGridLayout* mainLayout) {
        lightNameLabel = new QLabel("Light Name:");
        mainLayout->addWidget(lightNameLabel, 0, 0);

        lightNameText = new QLineEdit();
        mainLayout->addWidget(lightNameText, 0, 1);
        lightNameText->setFixedWidth(405);
    }

    void NewLightDialog::setupLightTypeFields(QGridLayout* mainLayout) {
        lightTypeLabel = new QLabel("Light Type:");
        mainLayout->addWidget(lightTypeLabel, 1, 0);

        lightTypeComboBox = new QComboBox();
        mainLayout->addWidget(lightTypeComboBox, 1, 1);
        lightTypeComboBox->setFixedWidth(150);
        lightTypeComboBox->addItem(OMNIDIRECTIONAL_LIGHT_LABEL, QVariant(Light::LightType::OMNIDIRECTIONAL));
        lightTypeComboBox->addItem(SUN_LIGHT_LABEL, QVariant(Light::LightType::SUN));
    }

    void NewLightDialog::updateLightName() {
        QString lightName = lightNameText->text();
        if (!lightName.isEmpty()) {
            this->lightName = lightName.toUtf8().constData();
        }
    }

    int NewLightDialog::buildSceneLight(int result) {
        sceneLight = new SceneLight();

        sceneLight->setName(lightName);

        QVariant variant = lightTypeComboBox->currentData();
        auto lightType = static_cast<Light::LightType>(variant.toInt());

        Light *light;
        if (lightType==Light::OMNIDIRECTIONAL) {
            light = new OmnidirectionalLight(Point3<float>(0.0, 0.0, 0.0));
        } else if (lightType==Light::SUN) {
            light = new SunLight(Vector3<float>(1.0, -1.0, 0.0));
        } else {
            throw std::invalid_argument("Unknown the light type to create a new light: " + std::to_string(lightType));
        }

        sceneLight->setLight(light);

        return result;
    }

    SceneLight* NewLightDialog::getSceneLight() const {
        return sceneLight;
    }

    void NewLightDialog::done(int r) {
        if (QDialog::Accepted == r) {
            bool hasError = false;

            updateLightName();
            LabelStyleHelper::applyNormalStyle(lightNameLabel);

            if (lightName.empty()) {
                LabelStyleHelper::applyErrorStyle(lightNameLabel, "Light name is mandatory");
                hasError = true;
            } else if (isSceneLightExist(lightName)) {
                LabelStyleHelper::applyErrorStyle(lightNameLabel, "Light name is already used");
                hasError = true;
            }

            if (!hasError) {
                r = buildSceneLight(r);
                QDialog::done(r);
            }
        } else {
            QDialog::done(r);
        }
    }

    bool NewLightDialog::isSceneLightExist(const std::string& name) {
        std::list<const SceneLight *> sceneLights = lightController->getSceneLights();
        return std::any_of(sceneLights.begin(), sceneLights.end(), [name](const auto& sl){return sl->getName() == name;});
    }

}
