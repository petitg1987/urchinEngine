#include <QMessageBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>
#include <UrchinAggregation.h>

#include "panel/terrain/dialog/NewTerrainDialog.h"
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
            terrainEntity(nullptr) {
        this->setWindowTitle("New Terrain");
        this->resize(530, 130);
        this->setFixedSize(this->width(), this->height());

        auto* mainLayout = new QGridLayout(this);
        mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        setupNameFields(mainLayout);
        setupHeightFilenameFields(mainLayout);

        auto* buttonBox = new QDialogButtonBox();
        mainLayout->addWidget(buttonBox, 2, 0, 1, 3);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
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

    int NewTerrainDialog::buildTerrainEntity(int result) {
        try {
            terrainEntity = std::make_unique<TerrainEntity>();

            terrainEntity->setName(terrainName);

            std::string resourcesDirectory = FileSystem::instance().getResourcesDirectory();
            std::string relativeHeightFilename = PathUtil::computeRelativePath(resourcesDirectory, heightFilename);
            std::vector<std::string> emptyMaterialFilenames;

            auto terrainMesh = std::make_unique<TerrainMesh>(relativeHeightFilename, 1.0f, 0.1f, TerrainMeshMode::NORMAL_BY_VERTEX);
            auto terrainMaterials = std::make_unique<TerrainMaterials>("", emptyMaterialFilenames, 1.0f, 1.0f);
            auto terrain = std::make_shared<Terrain>(std::move(terrainMesh), std::move(terrainMaterials), Point3(0.0f, 0.0f, 0.0f));

            terrainEntity->setTerrain(terrain);
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", e.what());
            return Rejected;
        }

        return result;
    }

    std::unique_ptr<TerrainEntity> NewTerrainDialog::moveTerrainEntity() {
        return std::move(terrainEntity);
    }

    void NewTerrainDialog::showHeightFilenameDialog() {
        QString directory = preferredHeightPath.isEmpty() ? QString::fromStdString(FileSystem::instance().getResourcesDirectory()) : preferredHeightPath;
        QString filename = QFileDialog::getOpenFileName(this, tr("Open image file"), directory, "Image file (*.png *.tga *.qoi)", nullptr, QFileDialog::DontUseNativeDialog);
        if (!filename.isNull()) {
            this->heightFilename = filename.toUtf8().constData();
            this->heightFilenameText->setText(filename);

            std::string preferredHeightPathString = FileUtil::getDirectory(heightFilename);
            preferredHeightPath = QString::fromStdString(preferredHeightPathString);
        }
    }

    void NewTerrainDialog::done(int r) {
        if (Accepted == r) {
            bool hasError = false;

            updateTerrainName();
            LabelStyleHelper::applyNormalStyle(terrainNameLabel);
            LabelStyleHelper::applyNormalStyle(heightFilenameLabel);

            if (terrainName.empty()) {
                LabelStyleHelper::applyErrorStyle(terrainNameLabel, "Terrain name is mandatory");
                hasError = true;
            } else if (isTerrainEntityExist(terrainName)) {
                LabelStyleHelper::applyErrorStyle(terrainNameLabel, "Terrain name is already used");
                hasError = true;
            }
            if (heightFilename.empty()) {
                LabelStyleHelper::applyErrorStyle(heightFilenameLabel, "Height is mandatory");
                hasError = true;
            }

            if (!hasError) {
                r = buildTerrainEntity(r);
                QDialog::done(r);
            }
        } else {
            QDialog::done(r);
        }
    }

    bool NewTerrainDialog::isTerrainEntityExist(std::string_view name) const {
        std::list<const TerrainEntity*> terrainEntities = terrainController->getTerrainEntities();
        return std::ranges::any_of(terrainEntities, [name](const auto& st){ return st->getName() == name; });
    }
}
