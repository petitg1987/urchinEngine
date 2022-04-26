#include <stdexcept>
#include <QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>
#include <UrchinCommon.h>

#include <panel/objects/dialog/NewObjectDialog.h>
#include <widget/style/LabelStyleHelper.h>
#include <widget/style/ButtonStyleHelper.h>

namespace urchin {

    QString NewObjectDialog::preferredMeshesPath = QString();

    NewObjectDialog::NewObjectDialog(QWidget* parent, const ObjectController* objectController) :
            QDialog(parent),
            objectController(objectController),
            objectNameLabel(nullptr),
            objectNameText(nullptr),
            meshesFilenameLabel(nullptr),
            meshesFilenameText(nullptr),
            objectEntity(nullptr) {
        this->setWindowTitle("New Object");
        this->resize(530, 130);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupNameFields(mainLayout);
        setupMeshFilenameFields(mainLayout);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 2, 0, 1, 3);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void NewObjectDialog::setupNameFields(QGridLayout* mainLayout) {
        objectNameLabel = new QLabel("Object Name:");
        mainLayout->addWidget(objectNameLabel, 0, 0);

        objectNameText = new QLineEdit();
        mainLayout->addWidget(objectNameText, 0, 1);
        objectNameText->setFixedWidth(360);
    }

    void NewObjectDialog::setupMeshFilenameFields(QGridLayout* mainLayout) {
        meshesFilenameLabel = new QLabel("Meshes File:");
        mainLayout->addWidget(meshesFilenameLabel, 1, 0);

        meshesFilenameText = new QLineEdit();
        mainLayout->addWidget(meshesFilenameText, 1, 1);
        meshesFilenameText->setReadOnly(true);
        meshesFilenameText->setFixedWidth(360);

        auto* selectMeshFileButton = new QPushButton("...");
        mainLayout->addWidget(selectMeshFileButton, 1, 2);
        ButtonStyleHelper::applyNormalStyle(selectMeshFileButton);
        selectMeshFileButton->setFixedWidth(22);
        connect(selectMeshFileButton, SIGNAL(clicked()), this, SLOT(showMeshFilenameDialog()));
    }

    void NewObjectDialog::updateObjectName() {
        QString objectName = objectNameText->text();
        if (!objectName.isEmpty()) {
            this->objectName = objectName.toUtf8().constData();
        }
    }

    int NewObjectDialog::buildObjectEntity(int result) {
        try {
            objectEntity = std::make_unique<ObjectEntity>();
            objectEntity->setName(objectName);
            objectEntity->setModel(Model::fromMeshesFile(meshesFilename));
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", e.what());
            return QDialog::Rejected;
        }

        return result;
    }

    std::unique_ptr<ObjectEntity> NewObjectDialog::moveObjectEntity() {
        return std::move(objectEntity);
    }

    void NewObjectDialog::showMeshFilenameDialog() {
        QString directory = preferredMeshesPath.isEmpty() ? QString::fromStdString(FileSystem::instance().getResourcesDirectory()) : preferredMeshesPath;
        QString filename = QFileDialog::getOpenFileName(this, tr("Open meshes file"), directory, "Meshes file (*.urchinMesh)", nullptr, QFileDialog::DontUseNativeDialog);
        if (!filename.isNull()) {
            this->meshesFilename = filename.toUtf8().constData();
            this->meshesFilenameText->setText(filename);

            std::string preferredMeshesPathString = FileUtil::getDirectory(meshesFilename);
            preferredMeshesPath = QString::fromStdString(preferredMeshesPathString);
        }
    }

    void NewObjectDialog::done(int r) {
        if (QDialog::Accepted == r) {
            bool hasError = false;

            updateObjectName();
            LabelStyleHelper::applyNormalStyle(objectNameLabel);
            LabelStyleHelper::applyNormalStyle(meshesFilenameLabel);

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

    bool NewObjectDialog::isObjectEntityExist(std::string_view name) const {
        std::list<const ObjectEntity*> objectEntities = objectController->getObjectEntities();
        return std::ranges::any_of(objectEntities, [name](const auto& so){ return so->getName() == name; });
    }

}
