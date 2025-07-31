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

    void ObjectTableView::selectRow(int row) {
        QModelIndex index = this->model()->index(row, 0);
        if (!index.isValid()) {
            return;
        }

        scrollTo(index);
        setCurrentIndex(index);
        selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
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

    bool ObjectTableView::hasObjectEntitySelected() const {
        return this->currentIndex().row() != -1 && this->selectionModel()->isSelected(currentIndex());
    }

    void ObjectTableView::selectObjectEntity(const ObjectEntity& expectedObjectEntity) {
        for (int rowId = 0; rowId < objectsListModel->rowCount(); ++rowId) {
            QModelIndex modelIndex = objectsListModel->index(rowId, 0);
            bool isObjectEntity = modelIndex.data(IS_OBJECT_ENTITY_DATA).value<bool>();
            if (isObjectEntity) {
                auto* objectEntity = modelIndex.data(GROUP_OR_OBJECT_ENTITY_DATA).value<const ObjectEntity*>();
                if (expectedObjectEntity.getName() == objectEntity->getName()) {
                    selectRow(rowId);
                    break;
                }
            }
        }
    }

    const ObjectEntity* ObjectTableView::getMainSelectedObjectEntity() const {
        if (hasObjectEntitySelected()) {
            return this->currentIndex().data(GROUP_OR_OBJECT_ENTITY_DATA).value<const ObjectEntity*>();
        }
        return nullptr;
    }

    std::vector<const ObjectEntity*> ObjectTableView::getAllSelectedObjectEntities() const {
        std::vector<const ObjectEntity*> objectEntities;
        objectEntities.reserve(this->selectedIndexes().size());
        for (const QModelIndex& modelIndex : this->selectedIndexes()) {
            objectEntities.push_back(modelIndex.data(GROUP_OR_OBJECT_ENTITY_DATA).value<const ObjectEntity*>());
        }
        return objectEntities;
    }

    void ObjectTableView::addObject(const ObjectEntity& objectEntity) {
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

        selectRow(insertRowId);
    }

    bool ObjectTableView::removeSelectedObject() const {
        if (hasObjectEntitySelected()) {
            objectsListModel->removeRow(this->currentIndex().row());
            return true;
        }
        return false;
    }

    bool ObjectTableView::updateSelectedObject(const ObjectEntity& updatedObjectEntity) const {
        if (hasObjectEntitySelected()) {
            const ObjectEntity* selectObjectEntity = getMainSelectedObjectEntity();
            if (selectObjectEntity == &updatedObjectEntity) {
                objectsListModel->setItem(currentIndex().row(), 0, buildObjectEntityItem(updatedObjectEntity));
                return true;
            } else {
                throw std::runtime_error("Update the selected object with a different instance is not allowed");
            }
        }
        return false;
    }

    void ObjectTableView::removeAllObjects() const {
        objectsListModel->removeRows(0, objectsListModel->rowCount());
    }

}
