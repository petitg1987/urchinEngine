#ifndef URCHINENGINE_MAPEDITOR_TERRAINTABLEVIEW_H
#define URCHINENGINE_MAPEDITOR_TERRAINTABLEVIEW_H

#include <QtWidgets/QTableView>
#include <QStandardItemModel>

#include "UrchinCommon.h"
#include "UrchinMapHandler.h"
#include "panel/SceneController.h"

Q_DECLARE_METATYPE(const urchin::SceneTerrain *)

namespace urchin
{

    class TerrainTableView : public QTableView, public Observable
    {
        Q_OBJECT

        public:
            explicit TerrainTableView(QWidget *parent = nullptr);

            enum NotificationType
            {
                SELECTION_CHANGED
            };

            bool hasSceneTerrainSelected() const;
            const SceneTerrain *getSelectedSceneTerrain() const;

            void addTerrain(const SceneTerrain *);
            bool removeSelectedTerrain();
            void removeAllTerrains();

        private:
            QStandardItemModel *terrainsListModel;

            void selectionChanged(const QItemSelection &, const QItemSelection &) override;
    };

}

#endif
