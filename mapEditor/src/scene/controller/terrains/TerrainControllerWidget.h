#ifndef URCHINENGINE_MAPEDITOR_TERRAINCONTROLLERWIDGET_H
#define URCHINENGINE_MAPEDITOR_TERRAINCONTROLLERWIDGET_H

#include <string>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>

#include "UrchinCommon.h"
#include "scene/controller/terrains/TerrainController.h"
#include "scene/controller/terrains/TerrainTableView.h"

namespace urchin
{

    class TerrainControllerWidget : public QWidget, public Observer
    {
        Q_OBJECT

        public:
            TerrainControllerWidget();
            ~TerrainControllerWidget() override = default;

            TerrainTableView *getTerrainTableView() const;

            void load(TerrainController *);
            void unload();

        private:
            void notify(Observable *, int) override;

            TerrainController *terrainController;

            TerrainTableView *terrainTableView;
            QPushButton *addTerrainButton;
            QPushButton *removeTerrainButton;

        private slots:
            void showAddTerrainDialog();
            void removeSelectedTerrain();
    };

}

#endif
