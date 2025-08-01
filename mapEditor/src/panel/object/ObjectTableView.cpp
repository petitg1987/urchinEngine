#include <QHeaderView>
#include <QVariant>
#include <UrchinAggregation.h>

#include "panel/object/ObjectTableView.h"

namespace urchin { //TODO review all methods for group

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
        itemGroup->setData(QString::fromStdString(groupName), GROUP_OR_OBJECT_ENTITY_DATA);
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

    bool ObjectTableView::hasObjectEntitiesSelected() const {
        if (currentIndex().row() == -1 || !selectionModel()->isSelected(currentIndex())) {
            return false;
        }

        QStandardItem* selectedItem = objectsListModel->itemFromIndex(currentIndex());
        if (selectedItem->data(IS_OBJECT_ENTITY_DATA).value<bool>()) {
            return true;
        } else {
            std::stack<QStandardItem*> allChildItems;
            allChildItems.push(selectedItem);

            while (!allChildItems.empty()) {
                QStandardItem* current = allChildItems.top();
                allChildItems.pop();

                if (current->data(IS_OBJECT_ENTITY_DATA).value<bool>()) {
                    return true;
                }

                for (int row = current->rowCount() - 1; row >= 0; --row) {
                    QStandardItem* child = current->child(row);
                    allChildItems.push(child);
                }
            }
        }
        return false;
    }

    std::vector<const ObjectEntity*> ObjectTableView::getAllSelectedObjectEntities() const {
        std::vector<const ObjectEntity*> objectEntities;
        objectEntities.reserve(this->selectedIndexes().size());
        for (const QModelIndex& modelIndex : this->selectedIndexes()) {
            objectEntities.push_back(modelIndex.data(GROUP_OR_OBJECT_ENTITY_DATA).value<const ObjectEntity*>());
        }
        return objectEntities;
    }

    void ObjectTableView::selectObjectEntity(const ObjectEntity& expectedObjectEntity) {
        std::stack<QStandardItem*> allChildItems;
        allChildItems.push(objectsListModel->invisibleRootItem());

        while (!allChildItems.empty()) {
            QStandardItem* current = allChildItems.top();
            allChildItems.pop();

            if (current->data(IS_OBJECT_ENTITY_DATA).value<bool>()) {
                auto* objectEntity = current->data(GROUP_OR_OBJECT_ENTITY_DATA).value<const ObjectEntity*>();
                if (expectedObjectEntity.getName() == objectEntity->getName()) {
                    selectItem(current->index());
                    return;
                }
            }

            for (int row = current->rowCount() - 1; row >= 0; --row) {
                QStandardItem* child = current->child(row);
                allChildItems.push(child);
            }
        }
    }

    void ObjectTableView::addObjectEntity(const ObjectEntity& objectEntity) {
        int insertRowId;
        for (insertRowId = 0; insertRowId < objectsListModel->rowCount(); ++insertRowId) {
            QModelIndex modelIndex = objectsListModel->index(insertRowId, 0);
            auto* existingObjectEntity = modelIndex.data(GROUP_OR_OBJECT_ENTITY_DATA).value<const ObjectEntity*>();
            if (objectEntity.getName().compare(existingObjectEntity->getName()) < 0) {
                break;
            }
        }

        objectsListModel->insertRow(insertRowId);
        objectsListModel->setItem(insertRowId, 0, buildObjectEntityItem(objectEntity));

        //TODO remove
        QModelIndex modelIndex = objectsListModel->index(insertRowId, 0);
        QStandardItem *standardItem = objectsListModel->itemFromIndex(modelIndex);
        standardItem->appendRow(buildGroupEntityItem("lol"));

        selectItem(this->model()->index(insertRowId, 0));
    }

    bool ObjectTableView::removeSelectedItem() const {
        if (hasObjectEntitiesSelected()) {
            objectsListModel->removeRow(this->currentIndex().row());
            return true;
        }
        return false;
    }

    bool ObjectTableView::refreshSelectedObjectEntity() const {
        if (hasMainObjectEntitySelected()) {
            const ObjectEntity* selectObjectEntity = getMainSelectedObjectEntity();
            objectsListModel->setItem(currentIndex().row(), 0, buildObjectEntityItem(*selectObjectEntity));
            return true;
        }
        return false;
    }

    void ObjectTableView::removeAll() const {
        objectsListModel->removeRows(0, objectsListModel->rowCount());
    }

}
