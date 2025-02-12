#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>

#include <panel/object/dialog/ChangeLightTypeDialog.h>

namespace urchin {

    ChangeLightTypeDialog::ChangeLightTypeDialog(QWidget* parent) :
                QDialog(parent),
                lightTypeComboBox(nullptr),
                lightType(std::nullopt) {
        this->setWindowTitle("Change Light Type");
        this->resize(260, 90);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupLightTypeFields(mainLayout);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 1, 0, 1, 2, Qt::AlignRight);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void ChangeLightTypeDialog::setupLightTypeFields(QGridLayout* mainLayout) {
        auto *lightTypeLabel = new QLabel("Light Type:");
        mainLayout->addWidget(lightTypeLabel, 0, 0);

        lightTypeComboBox = new QComboBox();
        mainLayout->addWidget(lightTypeComboBox, 0, 1);
        lightTypeComboBox->setFixedWidth(150);
        lightTypeComboBox->addItem(NONE_LABEL, QVariant(-1));
        lightTypeComboBox->addItem(SUN_LIGHT_LABEL, QVariant((int)Light::LightType::SUN));
        lightTypeComboBox->addItem(OMNIDIRECTIONAL_LIGHT_LABEL, QVariant((int)Light::LightType::OMNIDIRECTIONAL));
        lightTypeComboBox->addItem(SPOT_LIGHT_LABEL, QVariant((int)Light::LightType::SPOT));
    }

    void ChangeLightTypeDialog::done(int r) {
        if (Accepted == r) {
            int lightTypeIndex = lightTypeComboBox->currentData().toInt();
            if (lightTypeIndex == -1) {
                lightType = std::nullopt;
            } else {
                lightType = static_cast<Light::LightType>(lightTypeIndex);
            }
            QDialog::done(r);
        } else {
            QDialog::done(r);
        }
    }

    std::optional<Light::LightType> ChangeLightTypeDialog::getLightType() const {
        return lightType;
    }

}
