#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>
#include <QMessageBox>
#include <QSignalMapper>
#include <UrchinAggregation.h>

#include <panel/terrains/TerrainPanelWidget.h>
#include <widget/style/GroupBoxStyleHelper.h>
#include <widget/style/SpinBoxStyleHelper.h>
#include <widget/style/ButtonStyleHelper.h>
#include <panel/terrains/dialog/NewTerrainDialog.h>

namespace urchin {
    QString TerrainPanelWidget::preferredMaskMapPath = QString();
    QString TerrainPanelWidget::preferredMaterialPath = QString();
    QString TerrainPanelWidget::preferredGrassTexturePath = QString();
    QString TerrainPanelWidget::preferredGrassMaskPath = QString();

    TerrainPanelWidget::TerrainPanelWidget() :
            terrainController(nullptr),
            terrainTableView(nullptr),
            addTerrainButton(nullptr),
            removeTerrainButton(nullptr),
            generalPropertiesGroupBox(nullptr),
            meshGroupBox(nullptr),
            materialGroupBox(nullptr),
            grassGroupBox(nullptr),
            disableTerrainEvent(false),
            positionX(nullptr), positionY(nullptr), positionZ(nullptr),
            ambient(nullptr),
            xzScale(nullptr), yScale(nullptr),
            sRepeat(nullptr), tRepeat(nullptr),
            maskMapFilenameText(nullptr),
            grassTextureFilenameText(nullptr),
            grassMaskFilenameText(nullptr),
            numGrassInTex(nullptr),
            grassQuantity(nullptr), grassHeight(nullptr), grassWidth(nullptr),
            windDirectionX(nullptr), windDirectionY(nullptr), windDirectionZ(nullptr), windStrength(nullptr) {
        auto* mainLayout = new QVBoxLayout(this);
        mainLayout->setAlignment(Qt::AlignTop);
        mainLayout->setContentsMargins(1, 1, 1, 1);

        terrainTableView = new TerrainTableView();
        mainLayout->addWidget(terrainTableView);
        terrainTableView->addObserver(this, TerrainTableView::SELECTION_CHANGED);
        terrainTableView->setFixedHeight(320);

        auto* buttonsLayout = new QHBoxLayout();
        mainLayout->addLayout(buttonsLayout);
        buttonsLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        addTerrainButton = new QPushButton("New");
        buttonsLayout->addWidget(addTerrainButton);
        ButtonStyleHelper::applyNormalStyle(addTerrainButton);
        connect(addTerrainButton, SIGNAL(clicked()), this, SLOT(showAddTerrainDialog()));

        removeTerrainButton = new QPushButton("Remove");
        buttonsLayout->addWidget(removeTerrainButton);
        ButtonStyleHelper::applyNormalStyle(removeTerrainButton);
        removeTerrainButton->setEnabled(false);
        connect(removeTerrainButton, SIGNAL(clicked()), this, SLOT(removeSelectedTerrain()));

        setupGeneralPropertiesBox(mainLayout);
        setupMeshBox(mainLayout);
        setupMaterialBox(mainLayout);
        setupGrassBox(mainLayout);
    }

    void TerrainPanelWidget::setupGeneralPropertiesBox(QVBoxLayout* mainLayout) {
        generalPropertiesGroupBox = new QGroupBox("General Properties");
        mainLayout->addWidget(generalPropertiesGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(generalPropertiesGroupBox);
        generalPropertiesGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        generalPropertiesGroupBox->hide();

        auto* generalPropertiesLayout = new QGridLayout(generalPropertiesGroupBox);

        auto* positionLabel= new QLabel("Position:");
        generalPropertiesLayout->addWidget(positionLabel, 0, 0);

        auto* positionLayout = new QHBoxLayout();
        generalPropertiesLayout->addLayout(positionLayout, 0, 1);
        positionX = new QDoubleSpinBox();
        positionLayout->addWidget(positionX);
        SpinBoxStyleHelper::applyDefaultStyleOn(positionX);
        connect(positionX, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainGeneralProperties()));
        positionY = new QDoubleSpinBox();
        positionLayout->addWidget(positionY);
        SpinBoxStyleHelper::applyDefaultStyleOn(positionY);
        connect(positionY, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainGeneralProperties()));
        positionZ = new QDoubleSpinBox();
        positionLayout->addWidget(positionZ);
        SpinBoxStyleHelper::applyDefaultStyleOn(positionZ);
        connect(positionZ, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainGeneralProperties()));

        auto* ambientLabel= new QLabel("Ambient:");
        generalPropertiesLayout->addWidget(ambientLabel, 1, 0);

        ambient = new QDoubleSpinBox();
        generalPropertiesLayout->addWidget(ambient, 1, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(ambient);
        ambient->setMinimum(0.0);
        ambient->setMaximum(1.0);
        connect(ambient, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainGeneralProperties()));
    }

    void TerrainPanelWidget::setupMeshBox(QVBoxLayout* mainLayout) {
        meshGroupBox = new QGroupBox("Mesh");
        mainLayout->addWidget(meshGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(meshGroupBox);
        meshGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        meshGroupBox->hide();

        auto* meshLayout = new QGridLayout(meshGroupBox);

        auto* xzScaleLabel= new QLabel("XZ scale:");
        meshLayout->addWidget(xzScaleLabel, 0, 0);

        xzScale = new QDoubleSpinBox();
        meshLayout->addWidget(xzScale, 0, 1, 1, 3);
        SpinBoxStyleHelper::applyDefaultStyleOn(xzScale);
        xzScale->setMinimum(0.0);
        xzScale->setSingleStep(0.05);
        connect(xzScale, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainMesh()));

        auto* yScaleLabel= new QLabel("Y scale:");
        meshLayout->addWidget(yScaleLabel, 1, 0);

        yScale = new QDoubleSpinBox();
        meshLayout->addWidget(yScale, 1, 1, 1, 3);
        SpinBoxStyleHelper::applyDefaultStyleOn(yScale);
        yScale->setMinimum(0.0);
        yScale->setSingleStep(0.01);
        connect(yScale, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainMesh()));
    }

    void TerrainPanelWidget::setupMaterialBox(QVBoxLayout* mainLayout) {
        materialGroupBox = new QGroupBox("Material");
        mainLayout->addWidget(materialGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(materialGroupBox);
        materialGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        materialGroupBox->hide();

        auto* materialLayout = new QGridLayout(materialGroupBox);

        auto* repeatLabel= new QLabel("Repeat:");
        materialLayout->addWidget(repeatLabel, 0, 0);

        sRepeat = new QDoubleSpinBox();
        materialLayout->addWidget(sRepeat, 0, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(sRepeat);
        sRepeat->setMinimum(0.0);
        sRepeat->setSingleStep(1.0);
        connect(sRepeat, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainMaterial()));

        tRepeat = new QDoubleSpinBox();
        materialLayout->addWidget(tRepeat, 0, 2, 1, 3);
        SpinBoxStyleHelper::applyDefaultStyleOn(tRepeat);
        tRepeat->setMinimum(0.0);
        tRepeat->setSingleStep(1.0);
        connect(tRepeat, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainMaterial()));

        auto* maskMapLabel= new QLabel("Mask map:");
        materialLayout->addWidget(maskMapLabel, 1, 0);

        maskMapFilenameText = new QLineEdit();
        materialLayout->addWidget(maskMapFilenameText, 1, 1, 1, 2);
        maskMapFilenameText->setReadOnly(true);

        auto* selectMaskFileButton = new QPushButton("...");
        materialLayout->addWidget(selectMaskFileButton, 1, 3);
        ButtonStyleHelper::applyNormalStyle(selectMaskFileButton);
        selectMaskFileButton->setFixedWidth(22);
        connect(selectMaskFileButton, SIGNAL(clicked()), this, SLOT(showMaskFilenameDialog()));

        auto* clearMaskFileButton = new QPushButton("Clr");
        materialLayout->addWidget(clearMaskFileButton, 1, 4);
        ButtonStyleHelper::applyNormalStyle(clearMaskFileButton);
        clearMaskFileButton->setFixedWidth(22);
        connect(clearMaskFileButton, SIGNAL(clicked()), this, SLOT(clearMaskFilename()));

        materialFilenameTexts.resize(TerrainMaterials::MAX_MATERIAL);
        for (unsigned int i = 0; i < TerrainMaterials::MAX_MATERIAL; ++i) {
            std::string materialLabelStr = "Material " + std::to_string(i + 1) +":";
            auto* materialLabel= new QLabel(materialLabelStr.c_str());
            materialLayout->addWidget(materialLabel, 2 + (int)i, 0);

            materialFilenameTexts[i] = new QLineEdit();
            materialLayout->addWidget(materialFilenameTexts[i], 2 + (int)i, 1, 1, 2);
            materialFilenameTexts[i]->setReadOnly(true);

            auto* selectMaterialFileButton = new QPushButton("...");
            materialLayout->addWidget(selectMaterialFileButton, 2 + (int)i, 3);
            ButtonStyleHelper::applyNormalStyle(selectMaterialFileButton);
            selectMaterialFileButton->setFixedWidth(22);
            auto* selectSignalMapper = new QSignalMapper();
            connect(selectMaterialFileButton, SIGNAL(clicked()), selectSignalMapper, SLOT(map()));
            selectSignalMapper->setMapping(selectMaterialFileButton, QString::fromStdString(std::to_string(i)));
            connect(selectSignalMapper, SIGNAL(mapped(const QString&)), this, SLOT(showMaterialFilenameDialog(const QString&)));

            auto* clearMaterialFileButton = new QPushButton("Clr");
            materialLayout->addWidget(clearMaterialFileButton, 2 + (int)i, 4);
            ButtonStyleHelper::applyNormalStyle(clearMaterialFileButton);
            clearMaterialFileButton->setFixedWidth(22);
            auto* clearSignalMapper = new QSignalMapper();
            connect(clearMaterialFileButton, SIGNAL(clicked()), clearSignalMapper, SLOT(map()));
            clearSignalMapper->setMapping(clearMaterialFileButton, QString::fromStdString(std::to_string(i)));
            connect(clearSignalMapper, SIGNAL(mapped(const QString&)), this, SLOT(clearMaterialFilename(const QString&)));
        }
    }

    void TerrainPanelWidget::setupGrassBox(QVBoxLayout* mainLayout) {
        grassGroupBox = new QGroupBox("Grass");
        mainLayout->addWidget(grassGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(grassGroupBox);
        grassGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        grassGroupBox->hide();

        auto* grassLayout = new QGridLayout(grassGroupBox);

        auto* textureLabel= new QLabel("Texture:");
        grassLayout->addWidget(textureLabel, 1, 0);

        grassTextureFilenameText = new QLineEdit();
        grassLayout->addWidget(grassTextureFilenameText, 1, 1, 1, 3);
        grassTextureFilenameText->setReadOnly(true);

        auto* selectTextureFileButton = new QPushButton("...");
        grassLayout->addWidget(selectTextureFileButton, 1, 4);
        ButtonStyleHelper::applyNormalStyle(selectTextureFileButton);
        selectTextureFileButton->setFixedWidth(22);
        connect(selectTextureFileButton, SIGNAL(clicked()), this, SLOT(showGrassTextureFilenameDialog()));

        auto* clearTextureFileButton = new QPushButton("Clr");
        grassLayout->addWidget(clearTextureFileButton, 1, 5);
        ButtonStyleHelper::applyNormalStyle(clearTextureFileButton);
        clearTextureFileButton->setFixedWidth(22);
        connect(clearTextureFileButton, SIGNAL(clicked()), this, SLOT(clearGrassTextureFilename()));

        auto* maskLabel= new QLabel("Mask:");
        grassLayout->addWidget(maskLabel, 2, 0);

        grassMaskFilenameText = new QLineEdit();
        grassLayout->addWidget(grassMaskFilenameText, 2, 1, 1, 3);
        grassMaskFilenameText->setReadOnly(true);

        auto* selectMaskFileButton = new QPushButton("...");
        grassLayout->addWidget(selectMaskFileButton, 2, 4);
        ButtonStyleHelper::applyNormalStyle(selectMaskFileButton);
        selectMaskFileButton->setFixedWidth(22);
        connect(selectMaskFileButton, SIGNAL(clicked()), this, SLOT(showGrassMaskFilenameDialog()));

        auto* clearMaskFileButton = new QPushButton("Clr");
        grassLayout->addWidget(clearMaskFileButton, 2, 5);
        ButtonStyleHelper::applyNormalStyle(clearMaskFileButton);
        clearMaskFileButton->setFixedWidth(22);
        connect(clearMaskFileButton, SIGNAL(clicked()), this, SLOT(clearGrassMaskFilename()));

        auto* numGrassInTexLabel= new QLabel("Tex num grass:");
        grassLayout->addWidget(numGrassInTexLabel, 3, 0);

        numGrassInTex = new QSpinBox();
        grassLayout->addWidget(numGrassInTex, 3, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(numGrassInTex);
        numGrassInTex->setMinimum(1);
        numGrassInTex->setSingleStep(1);
        connect(numGrassInTex, SIGNAL(valueChanged(int)), this, SLOT(updateTerrainGrass()));

        auto* grassQuantityLabel= new QLabel("Grass quantity:");
        grassLayout->addWidget(grassQuantityLabel, 4, 0);

        grassQuantity = new QDoubleSpinBox();
        grassLayout->addWidget(grassQuantity, 4, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(grassQuantity);
        grassQuantity->setMinimum(0.0);
        grassQuantity->setSingleStep(0.1);
        connect(grassQuantity, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainGrass()));

        auto* grassHeightLabel= new QLabel("Height/width:");
        grassLayout->addWidget(grassHeightLabel, 5, 0);

        grassHeight = new QDoubleSpinBox();
        grassLayout->addWidget(grassHeight, 5, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(grassHeight);
        grassHeight->setMinimum(0.0);
        grassHeight->setSingleStep(0.05);
        connect(grassHeight, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainGrass()));

        grassWidth = new QDoubleSpinBox();
        grassLayout->addWidget(grassWidth, 5, 2);
        SpinBoxStyleHelper::applyDefaultStyleOn(grassWidth);
        grassWidth->setMinimum(0.0);
        grassWidth->setSingleStep(0.05);
        connect(grassWidth, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainGrass()));

        auto* windDirectionLabel= new QLabel("Wind direction:");
        grassLayout->addWidget(windDirectionLabel, 6, 0);

        windDirectionX = new QDoubleSpinBox();
        grassLayout->addWidget(windDirectionX, 6, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(windDirectionX);
        connect(windDirectionX, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainGrass()));
        windDirectionY = new QDoubleSpinBox();
        grassLayout->addWidget(windDirectionY, 6, 2);
        SpinBoxStyleHelper::applyDefaultStyleOn(windDirectionY);
        connect(windDirectionY, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainGrass()));
        windDirectionZ = new QDoubleSpinBox();
        grassLayout->addWidget(windDirectionZ, 6, 3, 1, 3);
        SpinBoxStyleHelper::applyDefaultStyleOn(windDirectionZ);
        connect(windDirectionZ, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainGrass()));

        auto* windStrengthLabel= new QLabel("Wind strength:");
        grassLayout->addWidget(windStrengthLabel, 7, 0);

        windStrength = new QDoubleSpinBox();
        grassLayout->addWidget(windStrength, 7, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(windStrength);
        windStrength->setMinimum(0.0);
        connect(windStrength, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainGrass()));
    }

    TerrainTableView* TerrainPanelWidget::getTerrainTableView() const {
        return terrainTableView;
    }

    void TerrainPanelWidget::load(TerrainController& terrainController) {
        this->terrainController = &terrainController;

        std::list<const TerrainEntity*> terrainEntities = this->terrainController->getTerrainEntities();
        for (auto& terrainEntity : terrainEntities) {
            terrainTableView->addTerrain(*terrainEntity);
        }
    }

    void TerrainPanelWidget::unload() {
        terrainTableView->removeAllTerrains();

        terrainController = nullptr;
    }

    void TerrainPanelWidget::notify(Observable* observable, int notificationType) {
        if (const auto* terrainTableView = dynamic_cast<TerrainTableView*>(observable)) {
            if (notificationType == TerrainTableView::SELECTION_CHANGED) {
                if (terrainTableView->hasTerrainEntitySelected()) {
                    const TerrainEntity* terrainEntity = terrainTableView->getSelectedTerrainEntity();
                    setupTerrainDataFrom(terrainEntity);

                    removeTerrainButton->setEnabled(true);
                    generalPropertiesGroupBox->show();
                    meshGroupBox->show();
                    materialGroupBox->show();
                    grassGroupBox->show();
                } else {
                    removeTerrainButton->setEnabled(false);
                    generalPropertiesGroupBox->hide();
                    meshGroupBox->hide();
                    materialGroupBox->hide();
                    grassGroupBox->hide();
                }
            }
        }
    }

    void TerrainPanelWidget::setupTerrainDataFrom(const TerrainEntity* terrainEntity) {
        disableTerrainEvent = true;
        const Terrain* terrain = terrainEntity->getTerrain();

        this->positionX->setValue(terrain->getPosition().X);
        this->positionY->setValue(terrain->getPosition().Y);
        this->positionZ->setValue(terrain->getPosition().Z);
        this->ambient->setValue(terrain->getAmbient());

        this->xzScale->setValue(terrain->getMesh()->getXZScale());
        this->yScale->setValue(terrain->getMesh()->getYScale());

        this->sRepeat->setValue(terrain->getMaterials()->getStRepeat().X);
        this->tRepeat->setValue(terrain->getMaterials()->getStRepeat().Y);
        std::string relativeMaskMapFilename = PathUtil::computeRelativePath(FileSystem::instance().getResourcesDirectory(), terrain->getMaterials()->getMaskMapFilename());
        this->maskMapFilenameText->setText(QString::fromStdString(relativeMaskMapFilename));
        this->materialFilenameTexts.resize(TerrainMaterials::MAX_MATERIAL);
        for (unsigned int i = 0; i < TerrainMaterials::MAX_MATERIAL; ++i) {
            auto& material = terrain->getMaterials()->getMaterials()[i];

            if (material != nullptr) {
                std::string relativeMtrFilename = PathUtil::computeRelativePath(FileSystem::instance().getResourcesDirectory(), material->getName());
                this->materialFilenameTexts[i]->setText(QString::fromStdString(relativeMtrFilename));
            } else {
                this->materialFilenameTexts[i]->setText("");
            }
        }

        auto& terrainGrass = terrain->getGrass();
        std::string relativeGrassTexFilename = PathUtil::computeRelativePath(FileSystem::instance().getResourcesDirectory(), terrainGrass.getGrassTexture());
        this->grassTextureFilenameText->setText(QString::fromStdString(relativeGrassTexFilename));
        std::string relativeGrassMaskFilename = PathUtil::computeRelativePath(FileSystem::instance().getResourcesDirectory(), terrainGrass.getMaskTexture());
        this->grassMaskFilenameText->setText(QString::fromStdString(relativeGrassMaskFilename));
        this->numGrassInTex->setValue((int)terrainGrass.getNumGrassInTexture());
        this->grassHeight->setValue(terrainGrass.getGrassHeight());
        this->grassWidth->setValue(terrainGrass.getGrassWidth());
        this->grassQuantity->setValue(terrainGrass.getGrassQuantity());
        this->windDirectionX->setValue(terrainGrass.getWindDirection().X);
        this->windDirectionY->setValue(terrainGrass.getWindDirection().Y);
        this->windDirectionZ->setValue(terrainGrass.getWindDirection().Z);
        this->windStrength->setValue(terrainGrass.getWindStrength());

        disableTerrainEvent = false;
    }

    void TerrainPanelWidget::showAddTerrainDialog() {
        NewTerrainDialog newTerrainEntityDialog(this, terrainController);
        newTerrainEntityDialog.exec();

        if (newTerrainEntityDialog.result() == QDialog::Accepted) {
            std::unique_ptr<TerrainEntity> terrainEntity = newTerrainEntityDialog.moveTerrainEntity();
            const TerrainEntity* terrainEntityPtr = terrainEntity.get();
            terrainController->addTerrainEntity(std::move(terrainEntity));

            int row = terrainTableView->addTerrain(*terrainEntityPtr);
            terrainTableView->selectRow(row);
        }
    }

    void TerrainPanelWidget::removeSelectedTerrain() {
        if (terrainTableView->hasTerrainEntitySelected()) {
            const TerrainEntity& terrainEntity = *terrainTableView->getSelectedTerrainEntity();
            terrainController->removeTerrainEntity(terrainEntity);

            terrainTableView->removeSelectedTerrain();
        }
    }

    void TerrainPanelWidget::updateTerrainGeneralProperties() {
        if (!disableTerrainEvent) {
            const TerrainEntity& terrainEntity = *terrainTableView->getSelectedTerrainEntity();

            Point3 position((float)positionX->value(), (float)positionY->value(), (float)positionZ->value());
            terrainController->updateTerrainGeneralProperties(terrainEntity, position, (float)ambient->value());
        }
    }

    void TerrainPanelWidget::updateTerrainMesh() {
        if (!disableTerrainEvent) {
            const TerrainEntity& terrainEntity = *terrainTableView->getSelectedTerrainEntity();

            terrainController->updateTerrainMesh(terrainEntity, (float)xzScale->value(), (float)yScale->value());
        }
    }

    void TerrainPanelWidget::updateTerrainMaterial() {
        if (!disableTerrainEvent) {
            const TerrainEntity& terrainEntity = *terrainTableView->getSelectedTerrainEntity();

            std::string maskMapFilename = maskMapFilenameText->text().toStdString();
            std::vector<std::string> materialFilenames;
            for (unsigned int i = 0; i < TerrainMaterials::MAX_MATERIAL; ++i) {
                materialFilenames.push_back(materialFilenameTexts[i]->text().toStdString());
            }
            terrainController->updateTerrainMaterial(terrainEntity, (float)sRepeat->value(), (float)tRepeat->value(), maskMapFilename, materialFilenames);
        }
    }

    void TerrainPanelWidget::updateTerrainGrass() {
        if (!disableTerrainEvent) {
            const TerrainEntity& terrainEntity = *terrainTableView->getSelectedTerrainEntity();

            std::string grassTextureFilename = grassTextureFilenameText->text().toStdString();
            std::string grassMaskFilename = grassMaskFilenameText->text().toStdString();
            auto numGrassInTexValue = (unsigned int)numGrassInTex->value();
            Vector3 windDirection((float)windDirectionX->value(), (float)windDirectionY->value(), (float)windDirectionZ->value());
            terrainController->updateTerrainGrass(terrainEntity, grassTextureFilename, grassMaskFilename, numGrassInTexValue, (float)grassQuantity->value(),
                                                       (float)grassHeight->value(), (float)grassWidth->value(), windDirection, (float)windStrength->value());
        }
    }

    void TerrainPanelWidget::showMaskFilenameDialog() {
        std::string resourcesDirectory = FileSystem::instance().getResourcesDirectory();
        QString directory = preferredMaskMapPath.isEmpty() ? QString::fromStdString(resourcesDirectory) : preferredMaskMapPath;
        QString filename = QFileDialog::getOpenFileName(this, tr("Open image file"), directory, "Image file (*.tga *.png *.qoi)", nullptr, QFileDialog::DontUseNativeDialog);
        if (!filename.isNull()) {
            std::string filenamePath = filename.toUtf8().constData();
            std::string relativeMaskMapFilename = PathUtil::computeRelativePath(resourcesDirectory, filenamePath);
            this->maskMapFilenameText->setText(QString::fromStdString(relativeMaskMapFilename));

            std::string preferredPathString = FileUtil::getDirectory(filenamePath);
            preferredMaskMapPath = QString::fromStdString(preferredPathString);

            try {
                updateTerrainMaterial();
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Error", e.what());
                clearMaskFilename();
            }
        }
    }

    void TerrainPanelWidget::clearMaskFilename() {
        this->maskMapFilenameText->setText("");

        updateTerrainMaterial();
    }

    void TerrainPanelWidget::showMaterialFilenameDialog(const QString& positionIndexStr) {
        auto i = (unsigned int)std::stoi(positionIndexStr.toStdString());

        std::string resourcesDirectory = FileSystem::instance().getResourcesDirectory();
        QString directory = preferredMaterialPath.isEmpty() ? QString::fromStdString(resourcesDirectory) : preferredMaterialPath;
        QString filename = QFileDialog::getOpenFileName(this, tr("Open material file"), directory, "Material file (*.uda)", nullptr, QFileDialog::DontUseNativeDialog);
        if (!filename.isNull()) {
            std::string mtrFilenamePath = filename.toUtf8().constData();
            std::string relativeMtrFilenamePath = PathUtil::computeRelativePath(resourcesDirectory, mtrFilenamePath);
            this->materialFilenameTexts[i]->setText(QString::fromStdString(relativeMtrFilenamePath));

            std::string preferredPathString = FileUtil::getDirectory(mtrFilenamePath);
            preferredMaterialPath = QString::fromStdString(preferredPathString);

            try {
                updateTerrainMaterial();
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Error", e.what());
                clearMaterialFilename(positionIndexStr);
            }
        }
    }

    void TerrainPanelWidget::clearMaterialFilename(const QString& positionIndexStr) {
        auto i = (unsigned int)std::stoi(positionIndexStr.toStdString());
        this->materialFilenameTexts[i]->setText("");

        updateTerrainMaterial();
    }

    void TerrainPanelWidget::showGrassTextureFilenameDialog() {
        std::string resourcesDirectory = FileSystem::instance().getResourcesDirectory();
        QString directory = preferredGrassTexturePath.isEmpty() ? QString::fromStdString(resourcesDirectory) : preferredGrassTexturePath;
        QString filename = QFileDialog::getOpenFileName(this, tr("Open image file"), directory, "Image file (*.tga *.png *.qoi)", nullptr, QFileDialog::DontUseNativeDialog);
        if (!filename.isNull()) {
            std::string filenamePath = filename.toUtf8().constData();
            std::string relativeGrassTexFilename = PathUtil::computeRelativePath(resourcesDirectory, filenamePath);
            this->grassTextureFilenameText->setText(QString::fromStdString(relativeGrassTexFilename));

            std::string preferredPathString = FileUtil::getDirectory(filenamePath);
            preferredGrassTexturePath = QString::fromStdString(preferredPathString);

            try {
                updateTerrainGrass();
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Error", e.what());
                clearMaskFilename();
            }
        }
    }

    void TerrainPanelWidget::clearGrassTextureFilename() {
        this->grassTextureFilenameText->setText("");

        updateTerrainGrass();
    }

    void TerrainPanelWidget::showGrassMaskFilenameDialog() {
        std::string resourcesDirectory = FileSystem::instance().getResourcesDirectory();
        QString directory = preferredGrassMaskPath.isEmpty() ? QString::fromStdString(resourcesDirectory) : preferredGrassMaskPath;
        QString filename = QFileDialog::getOpenFileName(this, tr("Open image file"), directory, "Image file (*.tga *.png *.qoi)", nullptr, QFileDialog::DontUseNativeDialog);
        if (!filename.isNull()) {
            std::string filenamePath = filename.toUtf8().constData();
            std::string relativeGrassTexFilename = PathUtil::computeRelativePath(resourcesDirectory, filenamePath);
            this->grassMaskFilenameText->setText(QString::fromStdString(relativeGrassTexFilename));

            std::string preferredPathString = FileUtil::getDirectory(filenamePath);
            preferredGrassMaskPath = QString::fromStdString(preferredPathString);

            try {
                updateTerrainGrass();
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "Error", e.what());
                clearMaskFilename();
            }
        }
    }

    void TerrainPanelWidget::clearGrassMaskFilename() {
        this->grassMaskFilenameText->setText("");

        updateTerrainGrass();
    }
}
