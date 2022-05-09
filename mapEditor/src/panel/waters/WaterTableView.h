#pragma once

#include <QtWidgets/QTableView>
#include <QStandardItemModel>

#include <UrchinCommon.h>
#include <UrchinAggregation.h>
#include <controller/SceneController.h>

Q_DECLARE_METATYPE(const urchin::WaterEntity*)

namespace urchin {

    class WaterTableView : public QTableView, public Observable {
        Q_OBJECT

        public:
            explicit WaterTableView(QWidget* = nullptr);

            enum NotificationType {
                SELECTION_CHANGED
            };

            bool hasWaterEntitySelected() const;
            const WaterEntity* getSelectedWaterEntity() const;

            int addWater(const WaterEntity&);
            bool removeSelectedWater();
            void removeAllWaters();

        private:
            QStandardItemModel* watersListModel;

            void selectionChanged(const QItemSelection&, const QItemSelection&) override;
    };

}
