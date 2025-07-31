#pragma once

#include <QtWidgets/QTreeView>
#include <QStandardItemModel>

#include <UrchinCommon.h>
#include <UrchinAggregation.h>

Q_DECLARE_METATYPE(const urchin::ObjectEntity*)

namespace urchin {

    class ObjectTableView final : public QTreeView, public Observable {
        Q_OBJECT

        public:
            explicit ObjectTableView(QWidget* = nullptr);

            enum NotificationType {
                OBJECT_SELECTION_CHANGED
            };

            void selectRow(int);

            bool hasObjectEntitySelected() const;
            const ObjectEntity* getMainSelectedObjectEntity() const;
            std::vector<const ObjectEntity*> getAllSelectedObjectEntities() const;

            int getObjectEntityRow(const ObjectEntity*) const;

            int addObject(const ObjectEntity&, std::size_t) const;
            bool removeSelectedObject() const;
            bool updateSelectedObject(const ObjectEntity&) const;
            void removeAllObjects() const;

        private:
            void selectionChanged(const QItemSelection&, const QItemSelection&) override;
            void currentChanged(const QModelIndex&, const QModelIndex&) override;
            QStandardItem* buildObjectEntityItem(const ObjectEntity&) const;

            QStandardItemModel* objectsListModel;
    };

}
