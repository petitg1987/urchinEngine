#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>

#include "panel/object/dialog/UpdateObjectDialog.h"
#include "widget/style/LabelStyleHelper.h"

namespace urchin {

    UpdateObjectDialog::UpdateObjectDialog(QWidget* parent, std::string originalName, const ObjectController* objectController) :
            QDialog(parent),
            originalName(std::move(originalName)),
            objectController(objectController),
            updatedNameLabel(nullptr),
            updatedNameText(nullptr) {
        this->setWindowTitle("Update");
        this->resize(530, 80);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupNameFields(mainLayout);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 2, 0, 1, 3);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void UpdateObjectDialog::setupNameFields(QGridLayout* mainLayout) {
        updatedNameLabel = new QLabel("Updated Name:");
        mainLayout->addWidget(updatedNameLabel, 0, 0);

        updatedNameText = new QLineEdit();
        mainLayout->addWidget(updatedNameText, 0, 1);
        updatedNameText->setFixedWidth(360);
        updatedNameText->setText(QString::fromStdString(originalName));
    }

    std::string UpdateObjectDialog::getUpdatedName() const {
        return updatedNameText->text().toStdString();
    }

    void UpdateObjectDialog::done(int r) {
        if (Accepted == r) {
            bool hasError = false;

            LabelStyleHelper::applyNormalStyle(updatedNameLabel);

            if (getUpdatedName().empty()) {
                LabelStyleHelper::applyErrorStyle(updatedNameLabel, "Object name is mandatory");
                hasError = true;
            } else if (getUpdatedName() != originalName && isObjectEntityExist(getUpdatedName())) {
                LabelStyleHelper::applyErrorStyle(updatedNameLabel, "Object name is already used");
                hasError = true;
            }

            if (!hasError) {
                QDialog::done(r);
            }
        } else {
            QDialog::done(r);
        }
    }

    bool UpdateObjectDialog::isObjectEntityExist(std::string_view name) const {
        std::list<const ObjectEntity*> objectEntities = objectController->getObjectEntities();
        return std::ranges::any_of(objectEntities, [name](const auto& so){ return so->getName() == name; });
    }

}
