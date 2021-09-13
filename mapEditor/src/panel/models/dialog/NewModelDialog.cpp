#include <stdexcept>
#include <QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>
#include <UrchinCommon.h>

#include <panel/models/dialog/NewModelDialog.h>
#include <widget/style/LabelStyleHelper.h>
#include <widget/style/ButtonStyleHelper.h>

namespace urchin {

    QString NewModelDialog::preferredMeshesPath = QString();

    NewModelDialog::NewModelDialog(QWidget* parent, const ModelController* modelController) :
            QDialog(parent),
            modelController(modelController),
            modelNameLabel(nullptr),
            modelNameText(nullptr),
            meshesFilenameLabel(nullptr),
            meshesFilenameText(nullptr),
            sceneModel(nullptr) {
        this->setWindowTitle("New Model");
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

    void NewModelDialog::setupNameFields(QGridLayout* mainLayout) {
        modelNameLabel = new QLabel("Model Name:");
        mainLayout->addWidget(modelNameLabel, 0, 0);

        modelNameText = new QLineEdit();
        mainLayout->addWidget(modelNameText, 0, 1);
        modelNameText->setFixedWidth(360);
    }

    void NewModelDialog::setupMeshFilenameFields(QGridLayout* mainLayout) {
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

    void NewModelDialog::updateModelName() {
        QString modelName = modelNameText->text();
        if (!modelName.isEmpty()) {
            this->modelName = modelName.toUtf8().constData();
        }
    }

    int NewModelDialog::buildSceneModel(int result) {
        try {
            sceneModel = std::make_unique<SceneModel>();

            sceneModel->setName(modelName);

            std::string resourcesDirectory = FileSystem::instance().getResourcesDirectory();
            std::string relativeMeshesFilename;
            if (!meshesFilename.empty()) {
                relativeMeshesFilename = FileUtil::getRelativePath(resourcesDirectory, meshesFilename);
            }
            auto model = Model::fromMeshesFile(relativeMeshesFilename);
            sceneModel->setModel(std::move(model));
        } catch (std::exception& e) {
            QMessageBox::critical(this, "Error", e.what());
            return QDialog::Rejected;
        }

        return result;
    }


    std::unique_ptr<SceneModel> NewModelDialog::moveSceneModel() {
        return std::move(sceneModel);
    }

    void NewModelDialog::showMeshFilenameDialog() {
        QString directory = preferredMeshesPath.isEmpty() ? QString::fromStdString(FileSystem::instance().getResourcesDirectory()) : preferredMeshesPath;
        QString filename = QFileDialog::getOpenFileName(this, tr("Open meshes file"), directory, "Meshes file (*.urchinMesh)", nullptr, QFileDialog::DontUseNativeDialog);
        if (!filename.isNull()) {
            this->meshesFilename = filename.toUtf8().constData();
            this->meshesFilenameText->setText(filename);

            std::string preferredMeshesPathString = FileUtil::getDirectory(meshesFilename);
            preferredMeshesPath = QString::fromStdString(preferredMeshesPathString);
        }
    }

    void NewModelDialog::done(int r) {
        if (QDialog::Accepted == r) {
            bool hasError = false;

            updateModelName();
            LabelStyleHelper::applyNormalStyle(modelNameLabel);
            LabelStyleHelper::applyNormalStyle(meshesFilenameLabel);

            if (modelName.empty()) {
                LabelStyleHelper::applyErrorStyle(modelNameLabel, "Model name is mandatory");
                hasError = true;
            } else if (isSceneModelExist(modelName)) {
                LabelStyleHelper::applyErrorStyle(modelNameLabel, "Model name is already used");
                hasError = true;
            }

            if (!hasError) {
                r = buildSceneModel(r);
                QDialog::done(r);
            }
        } else {
            QDialog::done(r);
        }
    }

    bool NewModelDialog::isSceneModelExist(const std::string& name) {
        std::list<const SceneModel*> sceneModels = modelController->getSceneModels();
        return std::any_of(sceneModels.begin(), sceneModels.end(), [&name](const auto& so){return so->getName() == name;});
    }

}
