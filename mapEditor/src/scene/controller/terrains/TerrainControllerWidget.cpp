#include <QtWidgets/QHBoxLayout>

#include "TerrainControllerWidget.h"
#include "support/GroupBoxStyleHelper.h"
#include "support/SpinBoxStyleHelper.h"
#include "support/ButtonStyleHelper.h"
#include "scene/controller/terrains/dialog/NewTerrainDialog.h"

namespace urchin
{
    TerrainControllerWidget::TerrainControllerWidget() :
            terrainController(nullptr)
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
                        //TODO setupLightDataFrom(sceneTerrain);

                        removeTerrainButton->setEnabled(true);
                        //TODO generalPropertiesGroupBox->show();
                    }else
                    {
                        removeTerrainButton->setEnabled(false);
                        //TODO generalPropertiesGroupBox->hide();
                    }
                    break;
                default:
                    ;
            }
        }
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
}
