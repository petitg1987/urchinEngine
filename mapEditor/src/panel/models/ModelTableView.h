#pragma once

#include <QtWidgets/QTableView>
#include <QStandardItemModel>

#include <UrchinCommon.h>
#include <UrchinMapHandler.h>
#include <controller/SceneController.h>

Q_DECLARE_METATYPE(const urchin::SceneModel*)

namespace urchin {

    class ModelTableView : public QTableView, public Observable {
        Q_OBJECT

        public:
            explicit ModelTableView(QWidget* = nullptr);

            enum NotificationType {
                MODEL_SELECTION_CHANGED
            };

            bool hasSceneModelSelected() const;
            const SceneModel* getSelectedSceneModel() const;

            int getSceneModelRow(const SceneModel*) const;

            int addModel(const SceneModel&);
            bool removeSelectedModel();
            void removeAllModels();

        private:
            QStandardItemModel* modelsListModel;

            void selectionChanged(const QItemSelection&, const QItemSelection&) override;
    };

}
