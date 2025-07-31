#pragma once

#include <QtWidgets/QTableView>
#include <QStandardItemModel>
#include <UrchinCommon.h>
#include <UrchinAggregation.h>

#include "controller/SceneController.h"

Q_DECLARE_METATYPE(const urchin::TerrainEntity*)

namespace urchin {

    class TerrainTableView final : public QTableView, public Observable {
        Q_OBJECT

        public:
            explicit TerrainTableView(QWidget* = nullptr);

            enum NotificationType {
                SELECTION_CHANGED
            };

            bool hasTerrainEntitySelected() const;
            const TerrainEntity* getSelectedTerrainEntity() const;

            int addTerrain(const TerrainEntity&);
            bool removeSelectedTerrain();
            void removeAllTerrains();

        private:
            QStandardItemModel* terrainsListModel;
    };

}
