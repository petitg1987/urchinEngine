#ifndef URCHINENGINE_WATERTABLEVIEW_H
#define URCHINENGINE_WATERTABLEVIEW_H

#include <QtWidgets/QTableView>
#include <QStandardItemModel>

#include "UrchinCommon.h"
#include "UrchinMapHandler.h"
#include "controller/SceneController.h"

Q_DECLARE_METATYPE(const urchin::SceneWater *)

namespace urchin {

    class WaterTableView : public QTableView, public Observable {
        Q_OBJECT

        public:
            explicit WaterTableView(QWidget *parent = nullptr);

            enum NotificationType {
                SELECTION_CHANGED
            };

            bool hasSceneWaterSelected() const;
            const SceneWater *getSelectedSceneWater() const;

            void addWater(const SceneWater *);
            bool removeSelectedWater();
            void removeAllWaters();

        private:
            QStandardItemModel *watersListModel;

            void selectionChanged(const QItemSelection &, const QItemSelection &) override;
    };

}

#endif
