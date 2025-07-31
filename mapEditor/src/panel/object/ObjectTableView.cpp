#include <QHeaderView>
#include <QVariant>
#include <UrchinAggregation.h>

#include "panel/object/ObjectTableView.h"

namespace urchin {

    ObjectTableView::ObjectTableView(QWidget* parent) :
            QTableView(parent) {
        objectsListModel = new QStandardItemModel(0, 1, this);
        objectsListModel->setHorizontalHeaderItem(0, new QStandardItem("Object Name"));
        QTableView::setModel(objectsListModel);

        horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
        horizontalHeader()->resizeSection(0, 340);
        verticalHeader()->hide();

        setSelectionMode(ExtendedSelection);
        setSelectionBehavior(SelectRows);
    }

    void ObjectTableView::selectionChanged(const QItemSelection&, const QItemSelection&) {
        //hack to refresh selection
        horizontalHeader()->resizeSection(0, 340);

        notifyObservers(this, OBJECT_SELECTION_CHANGED);
    }

    QStandardItem* ObjectTableView::buildObjectEntityItem(const ObjectEntity& objectEntity) const {
        auto* itemObjectName = new QStandardItem(QString::fromStdString(objectEntity.getName()));
        itemObjectName->setData(QVariant::fromValue(&objectEntity), Qt::UserRole + 1);
        itemObjectName->setEditable(false);
        return itemObjectName;
    }

    bool ObjectTableView::hasObjectEntitySelected() const {
        return this->currentIndex().row() != -1 && !this->selectedIndexes().empty();
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

    int ObjectTableView::addObject(const ObjectEntity& objectEntity, std::size_t insertPosition) {
        int insertRow = (int)insertPosition;
        if (insertRow > objectsListModel->rowCount()) {
            throw std::runtime_error("Unable to add an object at position " + std::to_string(insertPosition) + " while the list contains only "
                + std::to_string(objectsListModel->rowCount()) + " objects");
        }

        objectsListModel->insertRow(insertRow);
        objectsListModel->setItem(insertRow, 0, buildObjectEntityItem(objectEntity));

        resizeRowsToContents();
        return insertRow;
    }

    bool ObjectTableView::removeSelectedObject() {
        if (hasObjectEntitySelected()) {
            objectsListModel->removeRow(this->currentIndex().row());
            resizeRowsToContents();
            return true;
        }
        return false;
    }

    bool ObjectTableView::updateSelectedObject(const ObjectEntity& updatedObjectEntity) {
        if (hasObjectEntitySelected()) {
            const ObjectEntity* selectObjectEntity = getMainSelectedObjectEntity();
            if (selectObjectEntity == &updatedObjectEntity) {
                objectsListModel->setItem(currentIndex().row(), 0, buildObjectEntityItem(updatedObjectEntity));
                resizeRowsToContents();
                return true;
            } else {
                throw std::runtime_error("Update the selected object with a different instance is not allowed");
            }
        }
        return false;
    }

    void ObjectTableView::removeAllObjects() {
        objectsListModel->removeRows(0, objectsListModel->rowCount());
        resizeRowsToContents();
    }

}
