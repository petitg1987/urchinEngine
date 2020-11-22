#include <stdexcept>
#include <QMessageBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>

#include "CloneObjectDialog.h"
#include "widget/style/LabelStyleHelper.h"

namespace urchin {
    CloneObjectDialog::CloneObjectDialog(QWidget *parent, const ObjectController *objectController) :
            QDialog(parent),
            objectController(objectController),
            objectNameLabel(nullptr),
            objectNameText(nullptr),
            sceneObject(nullptr) {
        this->setWindowTitle("Clone Object");
        this->resize(530, 80);
        this->setFixedSize(this->width(),this->height());

        auto *mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupNameFields(mainLayout);

        auto *buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 2, 0, 1, 3);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void CloneObjectDialog::setupNameFields(QGridLayout *mainLayout) {
        objectNameLabel = new QLabel("Object Name:");
        mainLayout->addWidget(objectNameLabel, 0, 0);

        objectNameText = new QLineEdit();
        mainLayout->addWidget(objectNameText, 0, 1);
        objectNameText->setFixedWidth(360);
    }

    void CloneObjectDialog::updateObjectName() {
        QString objectName = objectNameText->text();
        if (!objectName.isEmpty()) {
            this->objectName = objectName.toUtf8().constData();
        }
    }

    int CloneObjectDialog::buildSceneObject(int result) {
        try {
            sceneObject = new SceneObject();
            sceneObject->setName(objectName);
        } catch (std::exception &e) {
            QMessageBox::critical(this, "Error", e.what());
            delete sceneObject;

            return QDialog::Rejected;
        }

        return result;
    }

    SceneObject *CloneObjectDialog::getSceneObject() const {
        return sceneObject;
    }

    void CloneObjectDialog::done(int r) {
        if (QDialog::Accepted == r) {
            bool hasError = false;

            updateObjectName();
            LabelStyleHelper::applyNormalStyle(objectNameLabel);

            if (objectName.empty()) {
                LabelStyleHelper::applyErrorStyle(objectNameLabel, "Object name is mandatory");
                hasError = true;
            } else if (isSceneObjectExist(objectName)) {
                LabelStyleHelper::applyErrorStyle(objectNameLabel, "Object name is already used");
                hasError = true;
            }

            if (!hasError) {
                r = buildSceneObject(r);
                QDialog::done(r);
            }
        } else {
            QDialog::done(r);
        }
    }

    bool CloneObjectDialog::isSceneObjectExist(const std::string &name) {
        std::list<const SceneObject *> sceneObjects = objectController->getSceneObjects();
        return std::any_of(sceneObjects.begin(), sceneObjects.end(), [name](const auto &so){return so->getName() == name;});
    }

}
