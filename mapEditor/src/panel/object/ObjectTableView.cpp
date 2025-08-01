#include <QHeaderView>
#include <QVariant>
#include <UrchinAggregation.h>

#include "panel/object/ObjectTableView.h"

namespace urchin {

    ObjectTableView::ObjectTableView(QWidget* parent) :
            QTreeView(parent) {
        objectsListModel = new QStandardItemModel(0, 1, this);
        objectsListModel->setHorizontalHeaderItem(0, new QStandardItem("Object Name"));
        QTreeView::setModel(objectsListModel);

        header()->setSectionResizeMode(0, QHeaderView::Fixed);
        header()->resizeSection(0, 340);

        setSelectionMode(ExtendedSelection);
        setSelectionBehavior(SelectRows);
    }

    void ObjectTableView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {
        QTreeView::selectionChanged(selected, deselected);
        notifyObservers(this, OBJECT_SELECTION_CHANGED);
    }

    void ObjectTableView::currentChanged(const QModelIndex& current, const QModelIndex& previous) {
        QTreeView::currentChanged(current, previous);
        notifyObservers(this, OBJECT_SELECTION_CHANGED);
    }

    void ObjectTableView::selectItem(const QModelIndex& itemIndex) {
        if (!itemIndex.isValid()) {
            return;
        }

        expand(itemIndex.parent());
        scrollTo(itemIndex);
        setCurrentIndex(itemIndex);
        selectionModel()->select(itemIndex, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }

    QStandardItem* ObjectTableView::buildGroupEntityItem(const std::string& groupName) const {
        auto* itemGroup = new QStandardItem(QString::fromStdString(groupName));
        itemGroup->setData(false, IS_OBJECT_ENTITY_DATA);
        itemGroup->setEditable(false);
        return itemGroup;
    }

    QStandardItem* ObjectTableView::buildObjectEntityItem(const ObjectEntity& objectEntity) const {
        auto* itemObjectEntity = new QStandardItem(QString::fromStdString(objectEntity.getName()));
        itemObjectEntity->setData(true, IS_OBJECT_ENTITY_DATA);
        itemObjectEntity->setData(QVariant::fromValue(&objectEntity), GROUP_OR_OBJECT_ENTITY_DATA);
        itemObjectEntity->setEditable(false);
        return itemObjectEntity;
    }

    bool ObjectTableView::hasMainObjectEntitySelected() const {
        if (currentIndex().row() == -1 || !selectionModel()->isSelected(currentIndex())) {
            return false;
        }

        QStandardItem* selectedItem = objectsListModel->itemFromIndex(currentIndex());
        return selectedItem->data(IS_OBJECT_ENTITY_DATA).value<bool>();
    }

    const ObjectEntity* ObjectTableView::getMainSelectedObjectEntity() const {
        if (hasMainObjectEntitySelected()) {
            return this->currentIndex().data(GROUP_OR_OBJECT_ENTITY_DATA).value<const ObjectEntity*>();
        }
        return nullptr;
    }

    std::vector<const ObjectEntity*> ObjectTableView::getAllSelectedObjectEntities() const {
        std::vector<const ObjectEntity*> objectEntities;

        for (const QModelIndex& modelIndex : selectedIndexes()) {
            std::stack<QStandardItem*> itemsToProcess;
            itemsToProcess.push(objectsListModel->itemFromIndex(modelIndex));

            while (!itemsToProcess.empty()) {
                QStandardItem* current = itemsToProcess.top();
                itemsToProcess.pop();

                if (current->data(IS_OBJECT_ENTITY_DATA).value<bool>()) {
                    objectEntities.push_back(current->data(GROUP_OR_OBJECT_ENTITY_DATA).value<const ObjectEntity*>());
                } else {
                    for (int row = current->rowCount() - 1; row >= 0; --row) {
                        QStandardItem* child = current->child(row);
                        itemsToProcess.push(child);
                    }
                }
            }
        }

        VectorUtil::removeDuplicates(objectEntities);
        return objectEntities;
    }

    void ObjectTableView::selectObjectEntity(const ObjectEntity& expectedObjectEntity) {
        std::stack<QStandardItem*> itemsToProcess;
        itemsToProcess.push(objectsListModel->invisibleRootItem());

        while (!itemsToProcess.empty()) {
            QStandardItem* current = itemsToProcess.top();
            itemsToProcess.pop();

            if (current->data(IS_OBJECT_ENTITY_DATA).value<bool>()) {
                auto* objectEntity = current->data(GROUP_OR_OBJECT_ENTITY_DATA).value<const ObjectEntity*>();
                if (expectedObjectEntity.getName() == objectEntity->getName()) {
                    selectItem(current->index());
                    return;
                }
            }

            for (int row = current->rowCount() - 1; row >= 0; --row) {
                QStandardItem* child = current->child(row);
                itemsToProcess.push(child);
            }
        }
    }

    void ObjectTableView::addObjectEntity(const ObjectEntity& objectEntity, bool selectNewObjectEntity) {
        QStandardItem* groupItem = findOrCreateGroupHierarchy(objectEntity.getGroupHierarchy());

        int childRow;
        for (childRow = 0; childRow < groupItem->rowCount(); ++childRow) {
            QStandardItem* child = groupItem->child(childRow);
            std::string childItemName;
            if (child->data(IS_OBJECT_ENTITY_DATA).value<bool>()) {
                childItemName = child->data(GROUP_OR_OBJECT_ENTITY_DATA).value<const ObjectEntity*>()->getName();
            } else {
                childItemName = child->text().toStdString();
            }

            if (objectEntity.getName().compare(childItemName) < 0) {
                break;
            }
        }

        QStandardItem* newObjectEntityItem = buildObjectEntityItem(objectEntity);
        groupItem->insertRow(childRow, newObjectEntityItem);
        if (selectNewObjectEntity) {
            selectItem(newObjectEntityItem->index());
        }
    }

    QStandardItem* ObjectTableView::findOrCreateGroupHierarchy(const std::vector<std::string>& groupHierarchy) const {
        QStandardItem* current = objectsListModel->invisibleRootItem();
        for (const std::string& group : groupHierarchy) {
            current = findOrCreateGroup(group, current);
        }
        return current;
    }

    QStandardItem* ObjectTableView::findOrCreateGroup(const std::string& group, QStandardItem* parent) const {
        int childRow;
        for (childRow = 0; childRow < parent->rowCount(); ++childRow) {
            QStandardItem* child = parent->child(childRow);
            std::string childItemName;
            if (child->data(IS_OBJECT_ENTITY_DATA).value<bool>()) {
                childItemName = child->data(GROUP_OR_OBJECT_ENTITY_DATA).value<const ObjectEntity*>()->getName();
            } else {
                childItemName = child->text().toStdString();
                if (childItemName == group) {
                    return child;
                }
            }

            if (group.compare(childItemName) < 0) {
                break;
            }
        }

        QStandardItem* newGroupItem = buildGroupEntityItem(group);
        parent->insertRow(childRow, newGroupItem);
        return newGroupItem;
    }

    void ObjectTableView::removeSelectedItems() const {
        //Use persistent index to avoid invalid index after first row removed
        QList<QPersistentModelIndex> persistentIndexes;
        QList<QPersistentModelIndex> persistentGroupIndexes;
        for (const QModelIndex &index : selectedIndexes()) {
            persistentIndexes.append(QPersistentModelIndex(index));
            persistentGroupIndexes.append(QPersistentModelIndex(index.parent()));
        }

        for (const QPersistentModelIndex &persistentIndex : persistentIndexes) {
            objectsListModel->removeRow(persistentIndex.row(), persistentIndex.parent());
        }

        //remove empty group
        for (const QPersistentModelIndex &persistentGroupIndex : persistentGroupIndexes) {
            std::optional<QModelIndex> topIndexToRemove = std::nullopt;
            if (!objectsListModel->hasChildren(persistentGroupIndex)) {
                topIndexToRemove = persistentGroupIndex;
                while (objectsListModel->rowCount(topIndexToRemove->parent()) <= 1) {
                    topIndexToRemove = topIndexToRemove->parent();
                }
            }
            if (topIndexToRemove) {
                objectsListModel->removeRow(topIndexToRemove->row(), topIndexToRemove->parent());
            }
        }
    }

    void ObjectTableView::refreshMainSelectedObjectEntity() const { //TODO impl correctly refresh of the name + group
        if (hasMainObjectEntitySelected()) {
            const ObjectEntity* selectObjectEntity = getMainSelectedObjectEntity();
            objectsListModel->setItem(currentIndex().row(), 0, buildObjectEntityItem(*selectObjectEntity));
        }
    }

    void ObjectTableView::removeAll() const {
        objectsListModel->removeRows(0, objectsListModel->rowCount());
    }

}
