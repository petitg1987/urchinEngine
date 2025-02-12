#pragma once

#include <QtWidgets/QTableView>
#include <QStandardItemModel>

#include <UrchinCommon.h>
#include <UrchinAggregation.h>
#include <controller/SceneController.h>

Q_DECLARE_METATYPE(const urchin::ObjectEntity*)

namespace urchin {

    class ObjectTableView final : public QTableView, public Observable {
        Q_OBJECT

        public:
            explicit ObjectTableView(QWidget* = nullptr);

            enum NotificationType {
                OBJECT_SELECTION_CHANGED
            };

            bool hasObjectEntitySelected() const;
            const ObjectEntity* getSelectedObjectEntity() const;
            bool isFirstObjectEntitySelected() const;
            bool isLastObjectEntitySelected() const;

            int getObjectEntityRow(const ObjectEntity*) const;

            int addObject(const ObjectEntity&);
            bool removeSelectedObject();
            bool updateSelectedObject(const ObjectEntity&);
            bool moveUpSelectedObject();
            bool moveDownSelectedObject();
            void removeAllObjects();

        private:
            void selectionChanged(const QItemSelection&, const QItemSelection&) override;
            std::vector<QStandardItem*> buildObjectEntityItems(const ObjectEntity&) const;

            QStandardItemModel* objectsListModel;
    };

}
