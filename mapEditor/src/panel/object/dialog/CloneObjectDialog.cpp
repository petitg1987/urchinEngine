#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>

#include <panel/object/dialog/CloneObjectDialog.h>
#include <widget/style/LabelStyleHelper.h>
#include <controller/EntityControllerUtil.h>

namespace urchin {

    CloneObjectDialog::CloneObjectDialog(QWidget* parent, const std::string& originalName, const ObjectController* objectController) :
            QDialog(parent),
            proposedName(EntityControllerUtil::determineNewCloneName(originalName, objectController->getObjectEntities())),
            objectController(objectController),
            objectNameLabel(nullptr),
            objectNameText(nullptr) {
        this->setWindowTitle("Clone Object");
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

    void CloneObjectDialog::setupNameFields(QGridLayout* mainLayout) {
        objectNameLabel = new QLabel("Object Name:");
        mainLayout->addWidget(objectNameLabel, 0, 0);

        objectNameText = new QLineEdit();
        mainLayout->addWidget(objectNameText, 0, 1);
        objectNameText->setFixedWidth(360);
        objectNameText->setText(QString::fromStdString(proposedName));
    }

    void CloneObjectDialog::updateObjectName() {
        QString objectName = objectNameText->text();
        if (!objectName.isEmpty()) {
            this->objectName = objectName.toUtf8().constData();
        }
    }

    const std::string& CloneObjectDialog::getObjectName() const {
        return objectName;
    }

    void CloneObjectDialog::done(int r) {
        if (Accepted == r) {
            bool hasError = false;

            updateObjectName();
            LabelStyleHelper::applyNormalStyle(objectNameLabel);

            if (objectName.empty()) {
                LabelStyleHelper::applyErrorStyle(objectNameLabel, "Object name is mandatory");
                hasError = true;
            } else if (isObjectEntityExist(objectName)) {
                LabelStyleHelper::applyErrorStyle(objectNameLabel, "Object name is already used");
                hasError = true;
            }

            if (!hasError) {
                QDialog::done(r);
            }
        } else {
            QDialog::done(r);
        }
    }

    bool CloneObjectDialog::isObjectEntityExist(std::string_view name) const {
        std::list<const ObjectEntity*> objectEntities = objectController->getObjectEntities();
        return std::ranges::any_of(objectEntities, [name](const auto& so){ return so->getName() == name; });
    }

}
