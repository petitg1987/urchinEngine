#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>

#include "widget/dialog/LightMaskNameDialog.h"

namespace urchin {

    LightMaskNameDialog::LightMaskNameDialog(QWidget* parent, const std::array<std::string, 8>& defaultNames) :
            QDialog(parent) {
        this->setWindowTitle("Light Mask Names");
        this->resize(530, 290);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        for (std::size_t i = 0; i < lightMaskNameTexts.size(); ++i) {
            auto* label = new QLabel(QString::fromStdString("Mask Name " + std::to_string(i + 1) + ":"));
            mainLayout->addWidget(label, (int)i, 0);

            lightMaskNameTexts[i] = new QLineEdit();
            mainLayout->addWidget(lightMaskNameTexts[i], (int)i, 1);
            lightMaskNameTexts[i]->setText(QString::fromStdString(defaultNames[i]));
        }

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, (int)lightMaskNameTexts.size(), 0, 1, 3, Qt::AlignRight);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    std::array<std::string, 8> LightMaskNameDialog::getLightMaskNames() const {
        std::array<std::string, 8> lightMaskNames;
        for (std::size_t i = 0; i < lightMaskNames.size(); ++i) {
            lightMaskNames[i] = lightMaskNameTexts[i]->text().toStdString();
        }
        return lightMaskNames;
    }

}
