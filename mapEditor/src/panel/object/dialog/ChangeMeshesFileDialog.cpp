#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>

#include "panel/object/dialog/ChangeMeshesFileDialog.h"
#include "widget/style/ButtonStyleHelper.h"

namespace urchin {

    QString ChangeMeshesFileDialog::preferredMeshesPath = QString();

    ChangeMeshesFileDialog::ChangeMeshesFileDialog(QWidget* parent, const std::string& initialMeshesFile) :
            QDialog(parent),
            meshesFilenameText(nullptr) {
        this->setWindowTitle("Change Meshes File");
        this->resize(530, 90);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupMeshesFilenameFields(mainLayout, initialMeshesFile);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 1, 0, 1, 4);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void ChangeMeshesFileDialog::setupMeshesFilenameFields(QGridLayout* mainLayout, const std::string& initialMeshesFile) {
        auto* meshesFilenameLabel = new QLabel("Meshes File:");
        mainLayout->addWidget(meshesFilenameLabel, 0, 0);

        meshesFilenameText = new QLineEdit();
        mainLayout->addWidget(meshesFilenameText, 0, 1);
        meshesFilenameText->setReadOnly(true);
        setupFilename(initialMeshesFile);

        auto* selectMeshesFileButton = new QPushButton("...");
        mainLayout->addWidget(selectMeshesFileButton, 0, 2);
        ButtonStyleHelper::applyDefaultStyle(selectMeshesFileButton);
        selectMeshesFileButton->setFixedWidth(22);
        connect(selectMeshesFileButton, SIGNAL(clicked()), this, SLOT(showMeshesFilenameDialog()));

        auto* clearMeshesFileButton = new QPushButton("Clear");
        mainLayout->addWidget(clearMeshesFileButton, 0, 3);
        ButtonStyleHelper::applyDefaultStyle(clearMeshesFileButton);
        clearMeshesFileButton->setFixedWidth(40);
        connect(clearMeshesFileButton, SIGNAL(clicked()), this, SLOT(clearMeshesFilename()));
    }

    void ChangeMeshesFileDialog::setupFilename(const std::string& filename) const {
        std::string relativeFilename = PathUtil::computeRelativePath(FileSystem::instance().getResourcesDirectory(), filename);
        meshesFilenameText->setText(QString::fromStdString(relativeFilename));

        if (!filename.empty()) {
            preferredMeshesPath = QString::fromStdString(FileUtil::getDirectory(filename));
        }
    }

    void ChangeMeshesFileDialog::showMeshesFilenameDialog() {
        QString directory = preferredMeshesPath.isEmpty() ? QString::fromStdString(FileSystem::instance().getResourcesDirectory()) : preferredMeshesPath;
        QString filename = QFileDialog::getOpenFileName(this, tr("Open meshes file"), directory, "Meshes file (*.urchinMesh)", nullptr, QFileDialog::DontUseNativeDialog);
        if (!filename.isNull()) {
            this->meshesFilename = filename.toStdString();
            setupFilename(this->meshesFilename);
        }
    }

    void ChangeMeshesFileDialog::clearMeshesFilename() {
        this->meshesFilename = "";
        setupFilename(this->meshesFilename);
    }

    std::string ChangeMeshesFileDialog::getMeshesFilename() const {
        return meshesFilename;
    }

}
