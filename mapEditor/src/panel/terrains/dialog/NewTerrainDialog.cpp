#include <stdexcept>
#include <QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>
#include "UrchinCommon.h"

#include "NewTerrainDialog.h"
#include "widget/style/LabelStyleHelper.h"
#include "widget/style/ButtonStyleHelper.h"

namespace urchin {
    QString NewTerrainDialog::preferredHeightPath = QString();

    NewTerrainDialog::NewTerrainDialog(QWidget* parent, const TerrainController* terrainController) :
            QDialog(parent),
            terrainController(terrainController),
            terrainNameLabel(nullptr),
            terrainNameText(nullptr),
            heightFilenameLabel(nullptr),
            heightFilenameText(nullptr),
            sceneTerrain(nullptr) {
        this->setWindowTitle("New Terrain");
        this->resize(530, 130);
        this->setFixedSize(this->width(),this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupNameFields(mainLayout);
        setupHeightFilenameFields(mainLayout);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 2, 0, 1, 3);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    }

    void NewTerrainDialog::setupNameFields(QGridLayout* mainLayout) {
        terrainNameLabel = new QLabel("Terrain Name:");
        mainLayout->addWidget(terrainNameLabel, 0, 0);

        terrainNameText = new QLineEdit();
        mainLayout->addWidget(terrainNameText, 0, 1);
        terrainNameText->setFixedWidth(360);
    }

    void NewTerrainDialog::setupHeightFilenameFields(QGridLayout* mainLayout) {
        heightFilenameLabel = new QLabel("Height File:");
        mainLayout->addWidget(heightFilenameLabel, 1, 0);

        heightFilenameText = new QLineEdit();
        mainLayout->addWidget(heightFilenameText, 1, 1);
        heightFilenameText->setReadOnly(true);
        heightFilenameText->setFixedWidth(360);

        auto* selectHeightFileButton = new QPushButton("...");
        mainLayout->addWidget(selectHeightFileButton, 1, 2);
        ButtonStyleHelper::applyNormalStyle(selectHeightFileButton);
        selectHeightFileButton->setFixedWidth(22);
        connect(selectHeightFileButton, SIGNAL(clicked()), this, SLOT(showHeightFilenameDialog()));
    }

    void NewTerrainDialog::updateTerrainName() {
        QString terrainName = terrainNameText->text();
        if (!terrainName.isEmpty()) {
            this->terrainName = terrainName.toUtf8().constData();
        }
    }

    int NewTerrainDialog::buildSceneTerrain(int result) {
        try {
            sceneTerrain = new SceneTerrain();

            sceneTerrain->setName(terrainName);

            std::string resourcesDirectory = FileSystem::instance()->getResourcesDirectory();
            std::string relativeHeightFilename = FileUtil::getRelativePath(resourcesDirectory, heightFilename);
            std::vector<std::string> emptyMaterialFilenames;

            auto terrainMesh = std::make_shared<TerrainMesh>(relativeHeightFilename, 1.0f, 0.1f);
            auto terrainMaterials = std::make_unique<TerrainMaterials>("", emptyMaterialFilenames, 1.0f, 1.0f);
            auto* terrain = new Terrain(terrainMesh, std::move(terrainMaterials), Point3<float>(0.0f, 0.0f, 0.0f));

            sceneTerrain->setTerrain(terrain);
        } catch (std::exception& e) {
            QMessageBox::critical(this, "Error", e.what());
            delete sceneTerrain;

            return QDialog::Rejected;
        }

        return result;
    }

    SceneTerrain* NewTerrainDialog::getSceneTerrain() const {
        return sceneTerrain;
    }

    void NewTerrainDialog::showHeightFilenameDialog() {
        QString directory = preferredHeightPath.isEmpty() ? QString::fromStdString(FileSystem::instance()->getResourcesDirectory()) : preferredHeightPath;
        QString filename = QFileDialog::getOpenFileName(this, tr("Open image file"), directory, "Image file (*.png *.tga)", nullptr, QFileDialog::DontUseNativeDialog);
        if (!filename.isNull()) {
            this->heightFilename = filename.toUtf8().constData();
            this->heightFilenameText->setText(filename);

            std::string preferredHeightPathString = FileUtil::getDirectory(heightFilename);
            preferredHeightPath = QString::fromStdString(preferredHeightPathString);
        }
    }

    void NewTerrainDialog::done(int r) {
        if (QDialog::Accepted == r) {
            bool hasError = false;

            updateTerrainName();
            LabelStyleHelper::applyNormalStyle(terrainNameLabel);
            LabelStyleHelper::applyNormalStyle(heightFilenameLabel);

            if (terrainName.empty()) {
                LabelStyleHelper::applyErrorStyle(terrainNameLabel, "Terrain name is mandatory");
                hasError = true;
            } else if (isSceneTerrainExist(terrainName)) {
                LabelStyleHelper::applyErrorStyle(terrainNameLabel, "Terrain name is already used");
                hasError = true;
            }
            if (heightFilename.empty()) {
                LabelStyleHelper::applyErrorStyle(heightFilenameLabel, "Height is mandatory");
                hasError = true;
            }

            if (!hasError) {
                r = buildSceneTerrain(r);
                QDialog::done(r);
            }
        } else {
            QDialog::done(r);
        }
    }

    bool NewTerrainDialog::isSceneTerrainExist(const std::string& name) {
        std::list<const SceneTerrain*> sceneTerrains = terrainController->getSceneTerrains();
        return std::any_of(sceneTerrains.begin(), sceneTerrains.end(), [&name](const auto& st){return st->getName() == name;});
    }
}
