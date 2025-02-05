#pragma once

#include <QtWidgets/QTableView>
#include <QStandardItemModel>

#include <UrchinCommon.h>
#include <UrchinAggregation.h>
#include <controller/SceneController.h>

Q_DECLARE_METATYPE(const urchin::LightEntity*)

namespace urchin {

    class LightTableView final : public QTableView, public Observable {
        Q_OBJECT

        public:
            explicit LightTableView(QWidget* = nullptr);

            enum NotificationType { //TODO remove
                LIGHT_SELECTION_CHANGED
            };

            bool hasLightEntitySelected() const;
            const LightEntity* getSelectedLightEntity() const;

            int addLight(const LightEntity&);
            bool removeSelectedLight();
            void removeAllLights();

        private:
            QStandardItemModel* lightsListModel;

            void selectionChanged(const QItemSelection&, const QItemSelection&) override;
    };

}
