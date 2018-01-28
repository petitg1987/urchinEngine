#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFileDialog>
#include <QMessageBox>
#include <QSignalMapper>

#include "TerrainControllerWidget.h"
#include "support/GroupBoxStyleHelper.h"
#include "support/SpinBoxStyleHelper.h"
#include "support/ButtonStyleHelper.h"
#include "scene/controller/terrains/dialog/NewTerrainDialog.h"

namespace urchin
{
    QString TerrainControllerWidget::preferredMaskMapPath = QString();
    QString TerrainControllerWidget::preferredMaterialPath = QString();
    QString TerrainControllerWidget::preferredGrassTexturePath = QString();
    QString TerrainControllerWidget::preferredGrassMaskPath = QString();

    TerrainControllerWidget::TerrainControllerWidget() :
            terrainController(nullptr),
            disableTerrainEvent(false)
    {
        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->setAlignment(Qt::AlignTop);
        mainLayout->setContentsMargins(1, 1, 1, 1);

        terrainTableView = new TerrainTableView();
        mainLayout->addWidget(terrainTableView);
        terrainTableView->addObserver(this, TerrainTableView::SELECTION_CHANGED);
        terrainTableView->setFixedHeight(220);

        auto *buttonsLayout = new QHBoxLayout();
        mainLayout->addLayout(buttonsLayout);
        buttonsLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        addTerrainButton = new QPushButton("New Terrain");
        buttonsLayout->addWidget(addTerrainButton);
        ButtonStyleHelper::applyNormalStyle(addTerrainButton);
        connect(addTerrainButton, SIGNAL(clicked()), this, SLOT(showAddTerrainDialog()));

        removeTerrainButton = new QPushButton("Remove Terrain");
        buttonsLayout->addWidget(removeTerrainButton);
        ButtonStyleHelper::applyNormalStyle(removeTerrainButton);
        removeTerrainButton->setEnabled(false);
        connect(removeTerrainButton, SIGNAL(clicked()), this, SLOT(removeSelectedTerrain()));

        setupGeneralPropertiesBox(mainLayout);
        setupMeshBox(mainLayout);
        setupMaterialBox(mainLayout);
        setupGrassBox(mainLayout);
    }

    void TerrainControllerWidget::setupGeneralPropertiesBox(QVBoxLayout *mainLayout)
    {
        generalPropertiesGroupBox = new QGroupBox("General Properties");
        mainLayout->addWidget(generalPropertiesGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(generalPropertiesGroupBox);
        generalPropertiesGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        generalPropertiesGroupBox->hide();

        auto *generalPropertiesLayout = new QGridLayout(generalPropertiesGroupBox);

        QLabel *positionLabel= new QLabel("Position:");
        generalPropertiesLayout->addWidget(positionLabel, 0, 0);

        auto *positionLayout = new QHBoxLayout();
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

        QLabel *ambientLabel= new QLabel("Ambient:");
        generalPropertiesLayout->addWidget(ambientLabel, 1, 0);

        ambient = new QDoubleSpinBox();
        generalPropertiesLayout->addWidget(ambient, 1, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(ambient);
        ambient->setMinimum(0.0);
        ambient->setMaximum(1.0);
        connect(ambient, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainGeneralProperties()));
    }

    void TerrainControllerWidget::setupMeshBox(QVBoxLayout *mainLayout)
    {
        meshGroupBox = new QGroupBox("Mesh");
        mainLayout->addWidget(meshGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(meshGroupBox);
        meshGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        meshGroupBox->hide();

        auto *meshLayout = new QGridLayout(meshGroupBox);

        QLabel *xzScaleLabel= new QLabel("XZ scale:");
        meshLayout->addWidget(xzScaleLabel, 0, 0);

        xzScale = new QDoubleSpinBox();
        meshLayout->addWidget(xzScale, 0, 1, 1, 3);
        SpinBoxStyleHelper::applyDefaultStyleOn(xzScale);
        xzScale->setMinimum(0.0);
        xzScale->setSingleStep(0.05);
        connect(xzScale, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainMesh()));

        QLabel *yScaleLabel= new QLabel("Y scale:");
        meshLayout->addWidget(yScaleLabel, 1, 0);

        yScale = new QDoubleSpinBox();
        meshLayout->addWidget(yScale, 1, 1, 1, 3);
        SpinBoxStyleHelper::applyDefaultStyleOn(yScale);
        yScale->setMinimum(0.0);
        yScale->setSingleStep(0.01);
        connect(yScale, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainMesh()));
    }

    void TerrainControllerWidget::setupMaterialBox(QVBoxLayout *mainLayout)
    {
        materialGroupBox = new QGroupBox("Material");
        mainLayout->addWidget(materialGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(materialGroupBox);
        materialGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        materialGroupBox->hide();

        auto *materialLayout = new QGridLayout(materialGroupBox);

        QLabel *repeatLabel= new QLabel("Repeat:");
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

        QLabel *maskMapLabel= new QLabel("Mask map:");
        materialLayout->addWidget(maskMapLabel, 1, 0);

        maskMapFilenameText = new QLineEdit();
        materialLayout->addWidget(maskMapFilenameText, 1, 1, 1, 2);
        maskMapFilenameText->setReadOnly(true);

        QPushButton *selectMaskFileButton = new QPushButton("...");
        materialLayout->addWidget(selectMaskFileButton, 1, 3);
        ButtonStyleHelper::applyNormalStyle(selectMaskFileButton);
        selectMaskFileButton->setFixedWidth(22);
        connect(selectMaskFileButton, SIGNAL(clicked()), this, SLOT(showMaskFilenameDialog()));

        QPushButton *clearMaskFileButton = new QPushButton("Clr");
        materialLayout->addWidget(clearMaskFileButton, 1, 4);
        ButtonStyleHelper::applyNormalStyle(clearMaskFileButton);
        clearMaskFileButton->setFixedWidth(22);
        connect(clearMaskFileButton, SIGNAL(clicked()), this, SLOT(clearMaskFilename()));

        materialFilenameTexts.resize(MAX_MATERIAL);
        for(unsigned int i=0; i<MAX_MATERIAL; ++i)
        {
            std::string materialLabelStr = "Material " + std::to_string(i+1) +":";
            QLabel *materialLabel= new QLabel(materialLabelStr.c_str());
            materialLayout->addWidget(materialLabel, 2+i, 0);

            materialFilenameTexts[i] = new QLineEdit();
            materialLayout->addWidget(materialFilenameTexts[i], 2+i, 1, 1, 2);
            materialFilenameTexts[i]->setReadOnly(true);

            QPushButton *selectMaterialFileButton = new QPushButton("...");
            materialLayout->addWidget(selectMaterialFileButton, 2+i, 3);
            ButtonStyleHelper::applyNormalStyle(selectMaterialFileButton);
            selectMaterialFileButton->setFixedWidth(22);
            auto *selectSignalMapper = new QSignalMapper();
            connect(selectMaterialFileButton, SIGNAL(clicked()), selectSignalMapper, SLOT(map()));
            selectSignalMapper->setMapping(selectMaterialFileButton, QString::fromStdString(std::to_string(i)));
            connect(selectSignalMapper, SIGNAL(mapped(const QString &)), this, SLOT(showMaterialFilenameDialog(const QString &)));

            QPushButton *clearMaterialFileButton = new QPushButton("Clr");
            materialLayout->addWidget(clearMaterialFileButton, 2+i, 4);
            ButtonStyleHelper::applyNormalStyle(clearMaterialFileButton);
            clearMaterialFileButton->setFixedWidth(22);
            auto *clearSignalMapper = new QSignalMapper();
            connect(clearMaterialFileButton, SIGNAL(clicked()), clearSignalMapper, SLOT(map()));
            clearSignalMapper->setMapping(clearMaterialFileButton, QString::fromStdString(std::to_string(i)));
            connect(clearSignalMapper, SIGNAL(mapped(const QString &)), this, SLOT(clearMaterialFilename(const QString &)));
        }
    }

    void TerrainControllerWidget::setupGrassBox(QVBoxLayout *mainLayout)
    {
        grassGroupBox = new QGroupBox("Grass");
        mainLayout->addWidget(grassGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(grassGroupBox);
        grassGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        grassGroupBox->hide();

        auto *grassLayout = new QGridLayout(grassGroupBox);

        QLabel *textureLabel= new QLabel("Texture:");
        grassLayout->addWidget(textureLabel, 1, 0);

        grassTextureFilenameText = new QLineEdit();
        grassLayout->addWidget(grassTextureFilenameText, 1, 1, 1, 3);
        grassTextureFilenameText->setReadOnly(true);

        QPushButton *selectTextureFileButton = new QPushButton("...");
        grassLayout->addWidget(selectTextureFileButton, 1, 4);
        ButtonStyleHelper::applyNormalStyle(selectTextureFileButton);
        selectTextureFileButton->setFixedWidth(22);
        connect(selectTextureFileButton, SIGNAL(clicked()), this, SLOT(showGrassTextureFilenameDialog()));

        QPushButton *clearTextureFileButton = new QPushButton("Clr");
        grassLayout->addWidget(clearTextureFileButton, 1, 5);
        ButtonStyleHelper::applyNormalStyle(clearTextureFileButton);
        clearTextureFileButton->setFixedWidth(22);
        connect(clearTextureFileButton, SIGNAL(clicked()), this, SLOT(clearGrassTextureFilename()));

        QLabel *maskLabel= new QLabel("Mask:");
        grassLayout->addWidget(maskLabel, 2, 0);

        grassMaskFilenameText = new QLineEdit();
        grassLayout->addWidget(grassMaskFilenameText, 2, 1, 1, 3);
        grassMaskFilenameText->setReadOnly(true);

        QPushButton *selectMaskFileButton = new QPushButton("...");
        grassLayout->addWidget(selectMaskFileButton, 2, 4);
        ButtonStyleHelper::applyNormalStyle(selectMaskFileButton);
        selectMaskFileButton->setFixedWidth(22);
        connect(selectMaskFileButton, SIGNAL(clicked()), this, SLOT(showGrassMaskFilenameDialog()));

        QPushButton *clearMaskFileButton = new QPushButton("Clr");
        grassLayout->addWidget(clearMaskFileButton, 2, 5);
        ButtonStyleHelper::applyNormalStyle(clearMaskFileButton);
        clearMaskFileButton->setFixedWidth(22);
        connect(clearMaskFileButton, SIGNAL(clicked()), this, SLOT(clearGrassMaskFilename()));

        QLabel *numGrassInTexLabel= new QLabel("Num grass in tex:");
        grassLayout->addWidget(numGrassInTexLabel, 3, 0);

        numGrassInTex = new QSpinBox();
        grassLayout->addWidget(numGrassInTex, 3, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(numGrassInTex);
        numGrassInTex->setMinimum(1);
        numGrassInTex->setSingleStep(1);
        connect(numGrassInTex, SIGNAL(valueChanged(int)), this, SLOT(updateTerrainGrass()));

        QLabel *grassQuantityLabel= new QLabel("Grass quantity:");
        grassLayout->addWidget(grassQuantityLabel, 4, 0);

        grassQuantity = new QDoubleSpinBox();
        grassLayout->addWidget(grassQuantity, 4, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(grassQuantity);
        grassQuantity->setMinimum(0.0);
        grassQuantity->setSingleStep(0.1);
        connect(grassQuantity, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainGrass()));

        QLabel *grassHeightLabel= new QLabel("Height/length:");
        grassLayout->addWidget(grassHeightLabel, 5, 0);

        grassHeight = new QDoubleSpinBox();
        grassLayout->addWidget(grassHeight, 5, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(grassHeight);
        grassHeight->setMinimum(0.0);
        grassHeight->setSingleStep(0.05);
        connect(grassHeight, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainGrass()));

        grassLength = new QDoubleSpinBox();
        grassLayout->addWidget(grassLength, 5, 2);
        SpinBoxStyleHelper::applyDefaultStyleOn(grassLength);
        grassLength->setMinimum(0.0);
        grassLength->setSingleStep(0.05);
        connect(grassLength, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainGrass()));

        QLabel *windDirectionLabel= new QLabel("Wind direction:");
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

        QLabel *windStrengthLabel= new QLabel("Wind strength:");
        grassLayout->addWidget(windStrengthLabel, 7, 0);

        windStrength = new QDoubleSpinBox();
        grassLayout->addWidget(windStrength, 7, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(windStrength);
        windStrength->setMinimum(0.0);
        connect(windStrength, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainGrass()));
    }

    TerrainTableView *TerrainControllerWidget::getTerrainTableView() const
    {
        return terrainTableView;
    }

    void TerrainControllerWidget::load(TerrainController *terrainController)
    {
        this->terrainController = terrainController;

        std::list<const SceneTerrain *> sceneTerrains = terrainController->getSceneTerrains();
        for(std::list<const SceneTerrain *>::const_iterator it=sceneTerrains.begin(); it!=sceneTerrains.end(); ++it)
        {
            terrainTableView->addTerrain((*it));
        }
    }

    void TerrainControllerWidget::unload()
    {
        terrainTableView->removeAllTerrains();

        terrainController = nullptr;
    }

    void TerrainControllerWidget::notify(Observable *observable, int notificationType)
    {
        if(auto *terrainTableView = dynamic_cast<TerrainTableView *>(observable))
        {
            switch(notificationType)
            {
                case TerrainTableView::SELECTION_CHANGED:
                    if(terrainTableView->hasSceneTerrainSelected())
                    {
                        const SceneTerrain *sceneTerrain = terrainTableView->getSelectedSceneTerrain();
                        setupTerrainDataFrom(sceneTerrain);

                        removeTerrainButton->setEnabled(true);
                        generalPropertiesGroupBox->show();
                        meshGroupBox->show();
                        materialGroupBox->show();
                        grassGroupBox->show();
                    }else
                    {
                        removeTerrainButton->setEnabled(false);
                        generalPropertiesGroupBox->hide();
                        meshGroupBox->hide();
                        materialGroupBox->hide();
                        grassGroupBox->hide();
                    }
                    break;
                default:
                    ;
            }
        }
    }

    void TerrainControllerWidget::setupTerrainDataFrom(const SceneTerrain *sceneTerrain)
    {
        disableTerrainEvent = true;
        const Terrain *terrain = sceneTerrain->getTerrain();

        this->positionX->setValue(terrain->getPosition().X);
        this->positionY->setValue(terrain->getPosition().Y);
        this->positionZ->setValue(terrain->getPosition().Z);
        this->ambient->setValue(terrain->getAmbient());

        this->xzScale->setValue(terrain->getMesh()->getXZScale());
        this->yScale->setValue(terrain->getMesh()->getYScale());

        this->sRepeat->setValue(terrain->getMaterial()->getSRepeat());
        this->tRepeat->setValue(terrain->getMaterial()->getTRepeat());
        this->maskMapFilenameText->setText(QString::fromStdString(terrain->getMaterial()->getMaskMapFilename()));
        this->materialFilenameTexts.resize(MAX_MATERIAL);
        for(unsigned int i=0; i<MAX_MATERIAL; ++i)
        {
            Material *material = terrain->getMaterial()->getMaterials()[i];

            if(material != nullptr)
            {
                this->materialFilenameTexts[i]->setText(QString::fromStdString(material->getName()));
            }else
            {
                this->materialFilenameTexts[i]->setText("");
            }
        }

        if(terrain->getGrass()!=nullptr)
        {
            auto *terrainGrass = terrain->getGrass();

            this->grassTextureFilenameText->setText(QString::fromStdString(terrainGrass->getGrassTexture()));
            this->grassMaskFilenameText->setText(QString::fromStdString(terrainGrass->getMaskTexture()));
            this->numGrassInTex->setValue(terrainGrass->getNumGrassInTexture());
            this->grassHeight->setValue(terrainGrass->getGrassHeight());
            this->grassLength->setValue(terrainGrass->getGrassLength());
            this->grassQuantity->setValue(terrainGrass->getGrassQuantity());
            this->windDirectionX->setValue(terrainGrass->getWindDirection().X);
            this->windDirectionY->setValue(terrainGrass->getWindDirection().Y);
            this->windDirectionZ->setValue(terrainGrass->getWindDirection().Z);
            this->windStrength->setValue(terrainGrass->getWindStrength());
        }

        disableTerrainEvent = false;
    }

    void TerrainControllerWidget::showAddTerrainDialog()
    {
        NewTerrainDialog newSceneTerrainDialog(this, terrainController);
        newSceneTerrainDialog.exec();

        if(newSceneTerrainDialog.result()==QDialog::Accepted)
        {
            SceneTerrain *sceneTerrain = newSceneTerrainDialog.getSceneTerrain();
            terrainController->addSceneTerrain(sceneTerrain);

            terrainTableView->addTerrain(sceneTerrain);
        }
    }

    void TerrainControllerWidget::removeSelectedTerrain()
    {
        if(terrainTableView->hasSceneTerrainSelected())
        {
            const SceneTerrain *sceneTerrain = terrainTableView->getSelectedSceneTerrain();
            terrainController->removeSceneTerrain(sceneTerrain);

            terrainTableView->removeSelectedTerrain();
        }
    }

    void TerrainControllerWidget::updateTerrainGeneralProperties()
    {
        if(!disableTerrainEvent)
        {
            const SceneTerrain *sceneTerrain = terrainTableView->getSelectedSceneTerrain();

            Point3<float> position(positionX->value(), positionY->value(), positionZ->value());
            terrainController->updateSceneTerrainGeneralProperties(sceneTerrain, position, ambient->value());
        }
    }

    void TerrainControllerWidget::updateTerrainMesh()
    {
        if(!disableTerrainEvent)
        {
            const SceneTerrain *sceneTerrain = terrainTableView->getSelectedSceneTerrain();

            terrainController->updateSceneTerrainMesh(sceneTerrain, xzScale->value(), yScale->value());
        }
    }

    void TerrainControllerWidget::updateTerrainMaterial()
    {
        if(!disableTerrainEvent)
        {
            const SceneTerrain *sceneTerrain = terrainTableView->getSelectedSceneTerrain();

            std::string maskMapFilename = maskMapFilenameText->text().toStdString();
            std::vector<std::string> materialFilenames;
            for(unsigned int i=0; i<MAX_MATERIAL; ++i)
            {
                materialFilenames.push_back(materialFilenameTexts[i]->text().toStdString());
            }
            terrainController->updateSceneTerrainMaterial(sceneTerrain, sRepeat->value(), tRepeat->value(), maskMapFilename, materialFilenames);
        }
    }

    void TerrainControllerWidget::updateTerrainGrass()
    {
        if(!disableTerrainEvent)
        {
            const SceneTerrain *sceneTerrain = terrainTableView->getSelectedSceneTerrain();

            std::string grassTextureFilename = grassTextureFilenameText->text().toStdString();
            std::string grassMaskFilename = grassMaskFilenameText->text().toStdString();
            auto numGrassInTexValue = static_cast<unsigned int>(numGrassInTex->value());
            Vector3<float> windDirection(windDirectionX->value(), windDirectionY->value(), windDirectionZ->value());
            terrainController->updateSceneTerrainGrass(sceneTerrain, grassTextureFilename, grassMaskFilename, numGrassInTexValue, grassQuantity->value(),
                                                       grassHeight->value(), grassLength->value(), windDirection, windStrength->value());
        }
    }

    void TerrainControllerWidget::showMaskFilenameDialog()
    {
        std::string resourcesDirectory = FileSystem::instance()->getResourcesDirectory();
        QString directory = preferredMaskMapPath.isEmpty() ? QString::fromStdString(resourcesDirectory) : preferredMaskMapPath;
        QString filename = QFileDialog::getOpenFileName(this, tr("Open image file"), directory, "Image file (*.tga *.png)", nullptr, QFileDialog::DontUseNativeDialog);
        if(!filename.isNull())
        {
            std::string tgaFilenamePath = filename.toUtf8().constData();
            std::string relativeTgaFilenamePath = FileHandler::getRelativePath(resourcesDirectory, tgaFilenamePath);
            this->maskMapFilenameText->setText(QString::fromStdString(relativeTgaFilenamePath));

            std::string preferredPathString = FileHandler::getDirectoryFrom(tgaFilenamePath);
            preferredMaskMapPath = QString::fromStdString(preferredPathString);

            try
            {
                updateTerrainMaterial();
            }catch(std::exception &e)
            {
                QMessageBox::critical(this, "Error", e.what());
                clearMaskFilename();
            }
        }
    }

    void TerrainControllerWidget::clearMaskFilename()
    {
        this->maskMapFilenameText->setText("");

        updateTerrainMaterial();
    }

    void TerrainControllerWidget::showMaterialFilenameDialog(const QString &positionIndexStr)
    {
        int i = std::stoi(positionIndexStr.toStdString());

        std::string resourcesDirectory = FileSystem::instance()->getResourcesDirectory();
        QString directory = preferredMaterialPath.isEmpty() ? QString::fromStdString(resourcesDirectory) : preferredMaterialPath;
        QString filename = QFileDialog::getOpenFileName(this, tr("Open material file"), directory, "Material file (*.mtr)", nullptr, QFileDialog::DontUseNativeDialog);
        if(!filename.isNull())
        {
            std::string mtrFilenamePath = filename.toUtf8().constData();
            std::string relativeMtrFilenamePath = FileHandler::getRelativePath(resourcesDirectory, mtrFilenamePath);
            this->materialFilenameTexts[i]->setText(QString::fromStdString(relativeMtrFilenamePath));

            std::string preferredPathString = FileHandler::getDirectoryFrom(mtrFilenamePath);
            preferredMaterialPath = QString::fromStdString(preferredPathString);

            try
            {
                updateTerrainMaterial();
            }catch(std::exception &e)
            {
                QMessageBox::critical(this, "Error", e.what());
                clearMaterialFilename(positionIndexStr);
            }
        }
    }

    void TerrainControllerWidget::clearMaterialFilename(const QString &positionIndexStr)
    {
        int i = std::stoi(positionIndexStr.toStdString());
        this->materialFilenameTexts[i]->setText("");

        updateTerrainMaterial();
    }

    void TerrainControllerWidget::showGrassTextureFilenameDialog()
    {
        std::string resourcesDirectory = FileSystem::instance()->getResourcesDirectory();
        QString directory = preferredGrassTexturePath.isEmpty() ? QString::fromStdString(resourcesDirectory) : preferredGrassTexturePath;
        QString filename = QFileDialog::getOpenFileName(this, tr("Open image file"), directory, "Image file (*.tga *.png)", nullptr, QFileDialog::DontUseNativeDialog);
        if(!filename.isNull())
        {
            std::string tgaFilenamePath = filename.toUtf8().constData();
            std::string relativeTgaFilenamePath = FileHandler::getRelativePath(resourcesDirectory, tgaFilenamePath);
            this->grassTextureFilenameText->setText(QString::fromStdString(relativeTgaFilenamePath));

            std::string preferredPathString = FileHandler::getDirectoryFrom(tgaFilenamePath);
            preferredGrassTexturePath = QString::fromStdString(preferredPathString);

            try
            {
                updateTerrainGrass();
            }catch(std::exception &e)
            {
                QMessageBox::critical(this, "Error", e.what());
                clearMaskFilename();
            }
        }
    }

    void TerrainControllerWidget::clearGrassTextureFilename()
    {
        this->grassTextureFilenameText->setText("");

        updateTerrainGrass();
    }

    void TerrainControllerWidget::showGrassMaskFilenameDialog()
    {
        std::string resourcesDirectory = FileSystem::instance()->getResourcesDirectory();
        QString directory = preferredGrassMaskPath.isEmpty() ? QString::fromStdString(resourcesDirectory) : preferredGrassMaskPath;
        QString filename = QFileDialog::getOpenFileName(this, tr("Open image file"), directory, "Image file (*.tga *.png)", nullptr, QFileDialog::DontUseNativeDialog);
        if(!filename.isNull())
        {
            std::string tgaFilenamePath = filename.toUtf8().constData();
            std::string relativeTgaFilenamePath = FileHandler::getRelativePath(resourcesDirectory, tgaFilenamePath);
            this->grassMaskFilenameText->setText(QString::fromStdString(relativeTgaFilenamePath));

            std::string preferredPathString = FileHandler::getDirectoryFrom(tgaFilenamePath);
            preferredGrassMaskPath = QString::fromStdString(preferredPathString);

            try
            {
                updateTerrainGrass();
            }catch(std::exception &e)
            {
                QMessageBox::critical(this, "Error", e.what());
                clearMaskFilename();
            }
        }
    }

    void TerrainControllerWidget::clearGrassMaskFilename()
    {
        this->grassMaskFilenameText->setText("");

        updateTerrainGrass();
    }
}
