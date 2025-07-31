#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>

#include "panel/object/dialog/AddObjectDialog.h"
#include "widget/style/LabelStyleHelper.h"
#include "widget/style/ButtonStyleHelper.h"

namespace urchin {

    AddObjectDialog::AddObjectDialog(QWidget* parent, const ObjectController* objectController) :
            QDialog(parent),
            objectController(objectController),
            objectNameLabel(nullptr),
            objectNameText(nullptr),
            objectEntity(nullptr) {
        this->setWindowTitle("New Object");
        this->resize(530, 90);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupNameFields(mainLayout);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 2, 0, 1, 2);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void AddObjectDialog::setupNameFields(QGridLayout* mainLayout) {
        objectNameLabel = new QLabel("Object Name:");
        mainLayout->addWidget(objectNameLabel, 0, 0);

        objectNameText = new QLineEdit();
        mainLayout->addWidget(objectNameText, 0, 1);
        objectNameText->setFixedWidth(360);
    }

    void AddObjectDialog::updateObjectName() {
        QString objectName = objectNameText->text();
        if (!objectName.isEmpty()) {
            this->objectName = objectName.toUtf8().constData();
        }
    }

    int AddObjectDialog::buildObjectEntity(int result) {
        objectEntity = std::make_unique<ObjectEntity>();
        objectEntity->setModel(Model::fromMeshesFile(""));
        objectEntity->setName(objectName);
        return result;
    }

    std::unique_ptr<ObjectEntity> AddObjectDialog::moveObjectEntity() {
        return std::move(objectEntity);
    }

    void AddObjectDialog::done(int r) {
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
                r = buildObjectEntity(r);
                QDialog::done(r);
            }
        } else {
            QDialog::done(r);
        }
    }

    bool AddObjectDialog::isObjectEntityExist(std::string_view name) const {
        std::list<const ObjectEntity*> objectEntities = objectController->getObjectEntities();
        return std::ranges::any_of(objectEntities, [name](const auto& so){ return so->getName() == name; });
    }

}
