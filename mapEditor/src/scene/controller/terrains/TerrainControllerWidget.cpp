#include <QtWidgets/QHBoxLayout>

#include "TerrainControllerWidget.h"
#include "support/GroupBoxStyleHelper.h"
#include "support/SpinBoxStyleHelper.h"
#include "support/ButtonStyleHelper.h"
#include "scene/controller/terrains/dialog/NewTerrainDialog.h"

namespace urchin
{
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
        meshLayout->addWidget(xzScale, 0, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(xzScale);
        xzScale->setMinimum(0.0); //TODO review step & position in layout
        connect(xzScale, SIGNAL(valueChanged(double)), this, SLOT(updateTerrainMesh()));

        QLabel *yScaleLabel= new QLabel("Y scale:");
        meshLayout->addWidget(yScaleLabel, 1, 0);

        yScale = new QDoubleSpinBox();
        meshLayout->addWidget(yScale, 1, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(yScale);
        yScale->setMinimum(0.0); //TODO review step & position in layout
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

        //TODO ...
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
                    }else
                    {
                        removeTerrainButton->setEnabled(false);
                        generalPropertiesGroupBox->hide();
                        meshGroupBox->hide();
                        materialGroupBox->hide();
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

        //TODO ...

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
            //TODO ...
        }
    }
}
