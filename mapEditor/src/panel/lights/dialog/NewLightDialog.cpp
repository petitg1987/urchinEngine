#include <stdexcept>
#include <QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QVariant>

#include <UrchinCommon.h>
#include <panel/lights/dialog/NewLightDialog.h>
#include <widget/style/LabelStyleHelper.h>

namespace urchin {
    NewLightDialog::NewLightDialog(QWidget* parent, const LightController* lightController) :
            QDialog(parent),
            lightController(lightController),
            lightNameLabel(nullptr),
            lightNameText(nullptr),
            lightTypeLabel(nullptr),
            lightTypeComboBox(nullptr),
            lightEntity(nullptr) {
        this->setWindowTitle("New Light");
        this->resize(530, 130);
        this->setFixedSize(this->width(), this->height());

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

    int NewLightDialog::buildLightEntity(int result) {
        lightEntity = std::make_unique<LightEntity>();

        lightEntity->setName(lightName);

        QVariant variant = lightTypeComboBox->currentData();
        auto lightType = static_cast<Light::LightType>(variant.toInt());

        std::shared_ptr<Light> light;
        if (lightType == Light::OMNIDIRECTIONAL) {
            light = std::make_shared<OmnidirectionalLight>(Point3<float>(0.0f, 0.0f, 0.0f));
        } else if (lightType == Light::SUN) {
            light = std::make_shared<SunLight>(Vector3<float>(1.0f, -1.0f, 0.0f));
        } else {
            throw std::invalid_argument("Unknown the light type to create a new light: " + std::to_string(lightType));
        }

        lightEntity->setLight(light);

        return result;
    }

    std::unique_ptr<LightEntity> NewLightDialog::moveLightEntity() {
        return std::move(lightEntity);
    }

    void NewLightDialog::done(int r) {
        if (QDialog::Accepted == r) {
            bool hasError = false;

            updateLightName();
            LabelStyleHelper::applyNormalStyle(lightNameLabel);

            if (lightName.empty()) {
                LabelStyleHelper::applyErrorStyle(lightNameLabel, "Light name is mandatory");
                hasError = true;
            } else if (isLightEntityExist(lightName)) {
                LabelStyleHelper::applyErrorStyle(lightNameLabel, "Light name is already used");
                hasError = true;
            }

            if (!hasError) {
                r = buildLightEntity(r);
                QDialog::done(r);
            }
        } else {
            QDialog::done(r);
        }
    }

    bool NewLightDialog::isLightEntityExist(std::string_view name) {
        std::list<const LightEntity*> lightEntities = lightController->getLightEntities();
        return std::ranges::any_of(lightEntities, [&name](const auto& sl){return sl->getName() == name;});
    }

}
