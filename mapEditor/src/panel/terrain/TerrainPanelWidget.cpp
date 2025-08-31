#include <QtWidgets/QFileDialog>
#include <QMessageBox>
#include <QSignalMapper>
#include <UrchinAggregation.h>

#include "panel/terrain/TerrainPanelWidget.h"
#include "widget/style/GroupBoxStyleHelper.h"
#include "widget/style/SpinBoxStyleHelper.h"
#include "widget/style/ButtonStyleHelper.h"
#include "panel/terrain/dialog/NewTerrainDialog.h"

namespace urchin {
    QString TerrainPanelWidget::preferredMaskMapPath = QString();
    QString TerrainPanelWidget::preferredMaterialPath = QString();

    TerrainPanelWidget::TerrainPanelWidget() :
            terrainController(nullptr),
            terrainTableView(nullptr),
            addTerrainButton(nullptr),
            removeTerrainButton(nullptr),
            generalPropertiesGroupBox(nullptr),
            meshGroupBox(nullptr),
            materialGroupBox(nullptr),
            disableTerrainEvent(false),
            positionX(nullptr),
            positionY(nullptr),
            positionZ(nullptr),
            ambient(nullptr),
            lightMask(nullptr),
            xzScale(nullptr),
            yScale(nullptr),
            flatMode(nullptr),
            sRepeat(nullptr),
            tRepeat(nullptr),
            maskMapFilenameText(nullptr) {
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
        ButtonStyleHelper::applyDefaultStyle(addTerrainButton);
        connect(addTerrainButton, SIGNAL(clicked()), this, SLOT(showAddTerrainDialog()));

        removeTerrainButton = new QPushButton("Remove");
        buttonsLayout->addWidget(removeTerrainButton);
        ButtonStyleHelper::applyDefaultStyle(removeTerrainButton);
        removeTerrainButton->setEnabled(false);
        connect(removeTerrainButton, SIGNAL(clicked()), this, SLOT(removeSelectedTerrain()));

        setupGeneralPropertiesBox(mainLayout);
        setupMeshBox(mainLayout);
        setupMaterialBox(mainLayout);
    }

    void TerrainPanelWidget::setupGeneralPropertiesBox(QVBoxLayout* mainLayout) {
        generalPropertiesGroupBox = new QGroupBox("General Properties");
        mainLayout->addWidget(generalPropertiesGroupBox);
        GroupBoxStyleHelper::applyDefaultStyle(generalPropertiesGroupBox);
        generalPropertiesGroupBox->hide();

        auto* generalPropertiesLayout = new QGridLayout(generalPropertiesGroupBox);

        auto* positionLabel= new QLabel("Position:");
        generalPropertiesLayout->addWidget(positionLabel, 0, 0);

        auto* positionLayout = new QHBoxLayout();
        generalPropertiesLayout->addLayout(positionLayout, 0, 1);
        positionX = new QDoubleSpinBox();
        positionLayout->addWidget(positionX);
        SpinBoxStyleHelper::applyDefaultStyle(positionX);
        connect(positionX, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainGeneralProperties()));
        positionY = new QDoubleSpinBox();
        positionLayout->addWidget(positionY);
        SpinBoxStyleHelper::applyDefaultStyle(positionY);
        connect(positionY, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainGeneralProperties()));
        positionZ = new QDoubleSpinBox();
        positionLayout->addWidget(positionZ);
        SpinBoxStyleHelper::applyDefaultStyle(positionZ);
        connect(positionZ, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainGeneralProperties()));

        auto* ambientLabel= new QLabel("Ambient:");
        generalPropertiesLayout->addWidget(ambientLabel, 1, 0);

        ambient = new QDoubleSpinBox();
        generalPropertiesLayout->addWidget(ambient, 1, 1);
        SpinBoxStyleHelper::applyDefaultStyle(ambient, 0.0, 1.0);
        connect(ambient, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainGeneralProperties()));

        auto* lightMaskLabel= new QLabel("Light mask:");
        generalPropertiesLayout->addWidget(lightMaskLabel, 2, 0);

        lightMask = new BitsetComboBox(nullptr, 8, 255);
        generalPropertiesLayout->addWidget(lightMask, 2, 1);
        connect(lightMask, SIGNAL(onBitChanged()), this, SLOT(updateTerrainGeneralProperties()));
    }

    void TerrainPanelWidget::setupMeshBox(QVBoxLayout* mainLayout) {
        meshGroupBox = new QGroupBox("Mesh");
        mainLayout->addWidget(meshGroupBox);
        GroupBoxStyleHelper::applyDefaultStyle(meshGroupBox);
        meshGroupBox->hide();

        auto* meshLayout = new QGridLayout(meshGroupBox);

        auto* xzScaleLabel= new QLabel("XZ scale:");
        meshLayout->addWidget(xzScaleLabel, 0, 0);

        xzScale = new QDoubleSpinBox();
        meshLayout->addWidget(xzScale, 0, 1, 1, 3);
        SpinBoxStyleHelper::applyDefaultStyle(xzScale, 0.0, 1000.0, 0.05);
        connect(xzScale, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainMesh()));

        auto* yScaleLabel= new QLabel("Y scale:");
        meshLayout->addWidget(yScaleLabel, 1, 0);

        yScale = new QDoubleSpinBox();
        meshLayout->addWidget(yScale, 1, 1, 1, 3);
        SpinBoxStyleHelper::applyDefaultStyle(yScale, 0.0, 1000.0, 0.01);
        connect(yScale, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainMesh()));

        flatMode = new QCheckBox("Flat mode");
        meshLayout->addWidget(flatMode, 2, 0, 1, 4);
        connect(flatMode, SIGNAL(stateChanged(int)), this, SLOT(updateTerrainMesh()));
    }

    void TerrainPanelWidget::setupMaterialBox(QVBoxLayout* mainLayout) {
        materialGroupBox = new QGroupBox("Material");
        mainLayout->addWidget(materialGroupBox);
        GroupBoxStyleHelper::applyDefaultStyle(materialGroupBox);
        materialGroupBox->hide();

        auto* materialLayout = new QGridLayout(materialGroupBox);

        auto* repeatLabel= new QLabel("Repeat:");
        materialLayout->addWidget(repeatLabel, 0, 0);

        sRepeat = new QDoubleSpinBox();
        materialLayout->addWidget(sRepeat, 0, 1);
        SpinBoxStyleHelper::applyDefaultStyle(sRepeat, 0.0, 1.0);
        connect(sRepeat, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainMaterial()));

        tRepeat = new QDoubleSpinBox();
        materialLayout->addWidget(tRepeat, 0, 2, 1, 3);
        SpinBoxStyleHelper::applyDefaultStyle(tRepeat, 0.0, 1.0);
        connect(tRepeat, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainMaterial()));

        auto* maskMapLabel= new QLabel("Mask map:");
        materialLayout->addWidget(maskMapLabel, 1, 0);

        maskMapFilenameText = new QLineEdit();
        materialLayout->addWidget(maskMapFilenameText, 1, 1, 1, 2);
        maskMapFilenameText->setReadOnly(true);

        auto* selectMaskFileButton = new QPushButton("...");
        materialLayout->addWidget(selectMaskFileButton, 1, 3);
        ButtonStyleHelper::applyDefaultStyle(selectMaskFileButton);
        selectMaskFileButton->setFixedWidth(22);
        connect(selectMaskFileButton, SIGNAL(clicked()), this, SLOT(showMaskFilenameDialog()));

        auto* clearMaskFileButton = new QPushButton("Clr");
        materialLayout->addWidget(clearMaskFileButton, 1, 4);
        ButtonStyleHelper::applyDefaultStyle(clearMaskFileButton);
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
            ButtonStyleHelper::applyDefaultStyle(selectMaterialFileButton);
            selectMaterialFileButton->setFixedWidth(22);
            auto* selectSignalMapper = new QSignalMapper();
            connect(selectMaterialFileButton, SIGNAL(clicked()), selectSignalMapper, SLOT(map()));
            selectSignalMapper->setMapping(selectMaterialFileButton, QString::fromStdString(std::to_string(i)));
            connect(selectSignalMapper, SIGNAL(mapped(const QString&)), this, SLOT(showMaterialFilenameDialog(const QString&)));

            auto* clearMaterialFileButton = new QPushButton("Clr");
            materialLayout->addWidget(clearMaterialFileButton, 2 + (int)i, 4);
            ButtonStyleHelper::applyDefaultStyle(clearMaterialFileButton);
            clearMaterialFileButton->setFixedWidth(22);
            auto* clearSignalMapper = new QSignalMapper();
            connect(clearMaterialFileButton, SIGNAL(clicked()), clearSignalMapper, SLOT(map()));
            clearSignalMapper->setMapping(clearMaterialFileButton, QString::fromStdString(std::to_string(i)));
            connect(clearSignalMapper, SIGNAL(mapped(const QString&)), this, SLOT(clearMaterialFilename(const QString&)));
        }
    }

    TerrainTableView* TerrainPanelWidget::getTerrainTableView() const {
        return terrainTableView;
    }

    void TerrainPanelWidget::load(SceneController& sceneController) {
        this->terrainController = &sceneController.getTerrainController();

        std::list<const TerrainEntity*> terrainEntities = this->terrainController->getTerrainEntities();
        for (auto& terrainEntity : terrainEntities) {
            terrainTableView->addTerrain(*terrainEntity);
        }

        sceneController.addObserverAndTriggerNotify(this, SceneController::LIGHT_MASK_NAMES_UPDATED);
    }

    void TerrainPanelWidget::unload() {
        terrainTableView->removeAllTerrains();

        terrainController = nullptr;
    }

    void TerrainPanelWidget::notify(Observable* observable, int notificationType) {
        if (const auto* sceneController = dynamic_cast<SceneController*>(observable)) {
            if (notificationType == SceneController::LIGHT_MASK_NAMES_UPDATED) {
                lightMask->updateLabels(sceneController->getLightMaskNames());
            }
        } else if (const auto* terrainTableView = dynamic_cast<TerrainTableView*>(observable)) {
            if (notificationType == TerrainTableView::SELECTION_CHANGED) {
                if (terrainTableView->hasTerrainEntitySelected()) {
                    const TerrainEntity* terrainEntity = terrainTableView->getSelectedTerrainEntity();
                    setupTerrainDataFrom(terrainEntity);

                    removeTerrainButton->setEnabled(true);
                    generalPropertiesGroupBox->show();
                    meshGroupBox->show();
                    materialGroupBox->show();
                } else {
                    removeTerrainButton->setEnabled(false);
                    generalPropertiesGroupBox->hide();
                    meshGroupBox->hide();
                    materialGroupBox->hide();
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
        this->lightMask->setBitValues(terrain->getLightMask());

        this->xzScale->setValue(terrain->getMesh()->getXZScale());
        this->yScale->setValue(terrain->getMesh()->getYScale());
        this->flatMode->setChecked(terrain->getMesh()->getMode() == TerrainMeshMode::FLAT);

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

    void TerrainPanelWidget::removeSelectedTerrain() const {
        if (terrainTableView->hasTerrainEntitySelected()) {
            const TerrainEntity& terrainEntity = *terrainTableView->getSelectedTerrainEntity();
            terrainController->removeTerrainEntity(terrainEntity);

            terrainTableView->removeSelectedTerrain();
        }
    }

    void TerrainPanelWidget::updateTerrainGeneralProperties() const {
        if (!disableTerrainEvent) {
            const TerrainEntity& terrainEntity = *terrainTableView->getSelectedTerrainEntity();

            Point3 position((float)positionX->value(), (float)positionY->value(), (float)positionZ->value());
            uint8_t lightMaskValue = static_cast<uint8_t>(lightMask->getBitValues());
            terrainController->updateTerrainGeneralProperties(terrainEntity, position, (float)ambient->value(), lightMaskValue);
        }
    }

    void TerrainPanelWidget::updateTerrainMesh() const {
        if (!disableTerrainEvent) {
            const TerrainEntity& terrainEntity = *terrainTableView->getSelectedTerrainEntity();
            TerrainMeshMode mode = flatMode->isChecked() ? TerrainMeshMode::FLAT : TerrainMeshMode::SMOOTH;

            terrainController->updateTerrainMesh(terrainEntity, (float)xzScale->value(), (float)yScale->value(), mode);
        }
    }

    void TerrainPanelWidget::updateTerrainMaterial() const {
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

    void TerrainPanelWidget::showMaskFilenameDialog() {
        std::string resourcesDirectory = FileSystem::instance().getResourcesDirectory();
        QString directory = preferredMaskMapPath.isEmpty() ? QString::fromStdString(resourcesDirectory) : preferredMaskMapPath;
        QString filename = QFileDialog::getOpenFileName(this, tr("Open image file"), directory, "Image file (*.tga *.png *.qoi)", nullptr, QFileDialog::DontUseNativeDialog);
        if (!filename.isNull()) {
            std::string filenamePath = filename.toStdString();
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

    void TerrainPanelWidget::clearMaskFilename() const {
        this->maskMapFilenameText->setText("");

        updateTerrainMaterial();
    }

    void TerrainPanelWidget::showMaterialFilenameDialog(const QString& positionIndexStr) {
        auto i = (unsigned int)std::stoi(positionIndexStr.toStdString());

        std::string resourcesDirectory = FileSystem::instance().getResourcesDirectory();
        QString directory = preferredMaterialPath.isEmpty() ? QString::fromStdString(resourcesDirectory) : preferredMaterialPath;
        QString filename = QFileDialog::getOpenFileName(this, tr("Open material file"), directory, "Material file (*.uda)", nullptr, QFileDialog::DontUseNativeDialog);
        if (!filename.isNull()) {
            std::string mtrFilenamePath = filename.toStdString();
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

    void TerrainPanelWidget::clearMaterialFilename(const QString& positionIndexStr) const {
        auto i = (unsigned int)std::stoi(positionIndexStr.toStdString());
        this->materialFilenameTexts[i]->setText("");

        updateTerrainMaterial();
    }

}
