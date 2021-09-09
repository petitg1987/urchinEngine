#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>

#include <widget/dialog/NotSavedDialog.h>

namespace urchin {

    NotSavedDialog::NotSavedDialog(QWidget* parent) :
            QDialog(parent),
            bNeedSave(false) {
        this->setWindowTitle("Save Map");
        this->resize(300, 120);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        auto* saveMapLabel = new QLabel("A map is currently open but not saved.\n"
                "Would you like to save it ?");
        mainLayout->addWidget(saveMapLabel, 0, 0);

        buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 1, 0, Qt::AlignRight);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Yes|QDialogButtonBox::No|QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonClick(QAbstractButton*)));
    }

    bool NotSavedDialog::needSave() const {
        return bNeedSave;
    }

    void NotSavedDialog::buttonClick(QAbstractButton* button) {
        if (buttonBox->standardButton(button) == QDialogButtonBox::Yes) {
            bNeedSave = true;
            accept();
        } else if (buttonBox->standardButton(button) == QDialogButtonBox::No) {
            bNeedSave = false;
            accept();
        } else if (buttonBox->standardButton(button) == QDialogButtonBox::Cancel) {
            bNeedSave = false;
            reject();
        } else {
            reject();
        }
    }

}
