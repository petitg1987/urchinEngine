#include <QtWidgets/QDialogButtonBox>
#include <Urchin3dEngine.h>

#include <panel/objects/dialog/ChangeLightDialog.h>
#include <widget/style/LabelStyleHelper.h>

namespace urchin {

    ChangeLightDialog::ChangeLightDialog(QWidget* parent) :
                QDialog(parent),
                lightTypeLabel(nullptr),
                lightTypeComboBox(nullptr) {
        this->setWindowTitle("Change Light");
        this->resize(245, 80);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupLightTypeFields(mainLayout);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 2, 0, 1, 2, Qt::AlignRight);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void ChangeLightDialog::setupLightTypeFields(QGridLayout* mainLayout) {
        lightTypeLabel = new QLabel("Light Type:");
        mainLayout->addWidget(lightTypeLabel, 1, 0);

        lightTypeComboBox = new QComboBox();
        mainLayout->addWidget(lightTypeComboBox, 1, 1);
        lightTypeComboBox->setFixedWidth(150);
        lightTypeComboBox->addItem(SUN_LIGHT_LABEL, QVariant((int)Light::LightType::SUN));
        lightTypeComboBox->addItem(OMNIDIRECTIONAL_LIGHT_LABEL, QVariant((int)Light::LightType::OMNIDIRECTIONAL));
        lightTypeComboBox->addItem(SPOT_LIGHT_LABEL, QVariant((int)Light::LightType::SPOT));
    }

    void ChangeLightDialog::done(int r) {
        if (Accepted == r) {
            QVariant variant = lightTypeComboBox->currentData();
            lightType = static_cast<Light::LightType>(variant.toInt());
            QDialog::done(r);
        } else {
            QDialog::done(r);
        }
    }

    Light::LightType ChangeLightDialog::getLightType() const {
        return lightType;
    }

}
