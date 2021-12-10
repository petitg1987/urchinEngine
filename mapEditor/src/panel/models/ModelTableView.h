#pragma once

#include <QtWidgets/QTableView>
#include <QStandardItemModel>

#include <UrchinCommon.h>
#include <UrchinMapHandler.h>
#include <controller/SceneController.h>

Q_DECLARE_METATYPE(const urchin::ObjectEntity*)

namespace urchin {

    class ModelTableView : public QTableView, public Observable { //TODO rename
        Q_OBJECT

        public:
            explicit ModelTableView(QWidget* = nullptr);

            enum NotificationType {
                OBJECT_SELECTION_CHANGED
            };

            bool hasObjectEntitySelected() const;
            const ObjectEntity* getSelectedObjectEntity() const;

            int getObjectEntityRow(const ObjectEntity*) const;

            int addObject(const ObjectEntity&);
            bool removeSelectedObject();
            void removeAllObjects();

        private:
            QStandardItemModel* objectsListModel;

            void selectionChanged(const QItemSelection&, const QItemSelection&) override;
    };

}
