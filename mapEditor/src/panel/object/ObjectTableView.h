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

            bool hasMainObjectEntitySelected() const;
            const ObjectEntity* getMainSelectedObjectEntity() const;
            std::vector<const ObjectEntity*> getAllSelectedObjectEntities() const;
            void selectObjectEntity(const ObjectEntity&);

            void addObjectEntity(const ObjectEntity&, bool);
            void removeSelectedItems() const;
            bool refreshMainSelectedObjectEntity() const;
            void removeAll() const;

        private:
            void selectItem(const QModelIndex&);
            void selectionChanged(const QItemSelection&, const QItemSelection&) override;
            void currentChanged(const QModelIndex&, const QModelIndex&) override;
            QStandardItem* buildGroupEntityItem(const std::string&) const;
            QStandardItem* buildObjectEntityItem(const ObjectEntity&) const;
            QStandardItem* findOrCreateGroupHierarchy(const std::vector<std::string>&) const;
            QStandardItem* findOrCreateGroup(const std::string&, QStandardItem*) const;

            static constexpr int IS_OBJECT_ENTITY_DATA = Qt::UserRole + 1;
            static constexpr int GROUP_OR_OBJECT_ENTITY_DATA = Qt::UserRole + 2;

            QStandardItemModel* objectsListModel;
    };

}
