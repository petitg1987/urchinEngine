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

            bool hasObjectEntitySelected() const;
            const ObjectEntity* getMainSelectedObjectEntity() const;
            std::vector<const ObjectEntity*> getAllSelectedObjectEntities() const;
            void selectObjectEntity(const ObjectEntity&);

            void addObject(const ObjectEntity&);
            bool removeSelectedObject() const;
            bool updateSelectedObject(const ObjectEntity&) const;
            void removeAllObjects() const;

        private:
            void selectRow(int);
            void selectionChanged(const QItemSelection&, const QItemSelection&) override;
            void currentChanged(const QModelIndex&, const QModelIndex&) override;
            QStandardItem* buildGroupEntityItem(const std::string&) const;
            QStandardItem* buildObjectEntityItem(const ObjectEntity&) const;

            static constexpr int IS_OBJECT_ENTITY_DATA = Qt::UserRole + 1;
            static constexpr int GROUP_OR_OBJECT_ENTITY_DATA = Qt::UserRole + 2;

            QStandardItemModel* objectsListModel;
    };

}
