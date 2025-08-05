#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>

#include "panel/object/dialog/ChangeMeshesFileDialog.h"
#include "widget/style/ButtonStyleHelper.h"

namespace urchin {

    QString ChangeMeshesFileDialog::preferredMeshesPath = QString();

    ChangeMeshesFileDialog::ChangeMeshesFileDialog(QWidget* parent) :
            QDialog(parent),
            meshesFilenameText(nullptr) {
        this->setWindowTitle("Change Meshes File");
        this->resize(530, 90);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupMeshFilenameFields(mainLayout);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 1, 0, 1, 3);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void ChangeMeshesFileDialog::setupMeshFilenameFields(QGridLayout* mainLayout) {
        auto* meshesFilenameLabel = new QLabel("Meshes File:");
        mainLayout->addWidget(meshesFilenameLabel, 0, 0);

        meshesFilenameText = new QLineEdit();
        mainLayout->addWidget(meshesFilenameText, 0, 1);
        meshesFilenameText->setReadOnly(true);
        meshesFilenameText->setFixedWidth(360);

        auto* selectMeshFileButton = new QPushButton("...");
        mainLayout->addWidget(selectMeshFileButton, 0, 2);
        ButtonStyleHelper::applyDefaultStyle(selectMeshFileButton);
        selectMeshFileButton->setFixedWidth(22);
        connect(selectMeshFileButton, SIGNAL(clicked()), this, SLOT(showMeshFilenameDialog()));
    }

    void ChangeMeshesFileDialog::showMeshFilenameDialog() {
        QString directory = preferredMeshesPath.isEmpty() ? QString::fromStdString(FileSystem::instance().getResourcesDirectory()) : preferredMeshesPath;
        QString filename = QFileDialog::getOpenFileName(this, tr("Open meshes file"), directory, "Meshes file (*.urchinMesh)", nullptr, QFileDialog::DontUseNativeDialog);
        if (!filename.isNull()) {
            this->meshesFilename = filename.toUtf8().constData();
            this->meshesFilenameText->setText(filename);

            std::string preferredMeshesPathString = FileUtil::getDirectory(meshesFilename);
            preferredMeshesPath = QString::fromStdString(preferredMeshesPathString);
        }
    }

    std::string ChangeMeshesFileDialog::getMeshesFilename() const {
        return meshesFilename;
    }

}
