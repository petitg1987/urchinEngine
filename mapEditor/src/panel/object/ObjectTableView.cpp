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

    void ObjectTableView::selectRow(int row) {
        QModelIndex index = this->model()->index(row, 0);
        if (!index.isValid()) {
            return;
        }

        scrollTo(index);
        setCurrentIndex(index);
        selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }

    QStandardItem* ObjectTableView::buildObjectEntityItem(const ObjectEntity& objectEntity) const {
        auto* itemObjectName = new QStandardItem(QString::fromStdString(objectEntity.getName()));
        itemObjectName->setData(QVariant::fromValue(&objectEntity), Qt::UserRole + 1);
        itemObjectName->setEditable(false);
        return itemObjectName;
    }

    bool ObjectTableView::hasObjectEntitySelected() const {
        return this->currentIndex().row() != -1 && this->selectionModel()->isSelected(currentIndex());
    }

    int ObjectTableView::getObjectEntityRow(const ObjectEntity* expectedObjectEntity) const {
        for (int rowId = 0; rowId < objectsListModel->rowCount(); ++rowId) {
            QModelIndex index = objectsListModel->index(rowId, 0);
            auto* objectEntity = index.data(Qt::UserRole + 1).value<const ObjectEntity*>();
            if (expectedObjectEntity->getName() == objectEntity->getName()) {
                return rowId;
            }
        }

        return -1;
    }

    const ObjectEntity* ObjectTableView::getMainSelectedObjectEntity() const {
        if (hasObjectEntitySelected()) {
            return this->currentIndex().data(Qt::UserRole + 1).value<const ObjectEntity*>();
        }
        return nullptr;
    }

    std::vector<const ObjectEntity*> ObjectTableView::getAllSelectedObjectEntities() const {
        std::vector<const ObjectEntity*> objectEntities;
        objectEntities.reserve(this->selectedIndexes().size());
        for (const QModelIndex& modelIndex : this->selectedIndexes()) {
            objectEntities.push_back(modelIndex.data(Qt::UserRole + 1).value<const ObjectEntity*>());
        }
        return objectEntities;
    }

    int ObjectTableView::addObject(const ObjectEntity& objectEntity) const { //TODO insert at correct place: group + sorted
        objectsListModel->insertRow(0);
        objectsListModel->setItem(0, 0, buildObjectEntityItem(objectEntity));

        return 0;
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
