#include <QHeaderView>
#include <QVariant>
#include <UrchinAggregation.h>

#include <panel/objects/ObjectTableView.h>

namespace urchin {

    ObjectTableView::ObjectTableView(QWidget* parent) :
            QTableView(parent) {
        objectsListModel = new QStandardItemModel(0, 2, this);
        objectsListModel->setHorizontalHeaderItem(0, new QStandardItem("Object Name"));
        objectsListModel->setHorizontalHeaderItem(1, new QStandardItem("Meshes File"));
        QTableView::setModel(objectsListModel);

        horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
        horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
        horizontalHeader()->resizeSection(0, 180);
        horizontalHeader()->resizeSection(1, 160);
        verticalHeader()->hide();

        setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    }

    void ObjectTableView::selectionChanged(const QItemSelection&, const QItemSelection&) {
        //hack to refresh selection
        horizontalHeader()->resizeSection(0, 180);
        horizontalHeader()->resizeSection(0, 160);

        notifyObservers(this, NotificationType::OBJECT_SELECTION_CHANGED);
    }

    std::vector<QStandardItem*> ObjectTableView::buildObjectEntityItems(const ObjectEntity& objectEntity) const {
        std::vector<QStandardItem*> objectEntityItems;
        objectEntityItems.reserve(2);

        auto* itemObjectName = new QStandardItem(QString::fromStdString(objectEntity.getName()));
        itemObjectName->setToolTip(QString::fromStdString(objectEntity.getName()));
        itemObjectName->setData(QVariant::fromValue(&objectEntity), Qt::UserRole + 1);
        itemObjectName->setEditable(false);
        objectEntityItems.push_back(itemObjectName);

        std::string meshesName;
        std::string relativeMeshesFilename;
        if (objectEntity.getModel()->getConstMeshes()) {
            meshesName = objectEntity.getModel()->getConstMeshes()->getMeshesName();
            relativeMeshesFilename = PathUtil::computeRelativePath(FileSystem::instance().getResourcesDirectory(), objectEntity.getModel()->getConstMeshes()->getMeshesFilename());
        }
        auto* itemMeshName = new QStandardItem(QString::fromStdString(meshesName));
        itemMeshName->setToolTip(QString::fromStdString(relativeMeshesFilename));
        itemMeshName->setData(QVariant::fromValue(&objectEntity), Qt::UserRole + 1);
        itemMeshName->setEditable(false);
        objectEntityItems.push_back(itemMeshName);

        return objectEntityItems;
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

    const ObjectEntity* ObjectTableView::getSelectedObjectEntity() const {
        if (hasObjectEntitySelected()) {
            return this->currentIndex().data(Qt::UserRole + 1).value<const ObjectEntity*>();
        }
        return nullptr;
    }

    bool ObjectTableView::isFirstObjectEntitySelected() const {
        return hasObjectEntitySelected() && this->currentIndex().row() == 0;
    }

    bool ObjectTableView::isLastObjectEntitySelected() const {
        return hasObjectEntitySelected() && this->currentIndex().row() == objectsListModel->rowCount() - 1;
    }

    int ObjectTableView::addObject(const ObjectEntity& objectEntity) {
        int nextRow = objectsListModel->rowCount();
        objectsListModel->insertRow(nextRow);

        std::vector<QStandardItem*> objectEntityItems = buildObjectEntityItems(objectEntity);
        int column = 0;
        for (QStandardItem* objectEntityItem : objectEntityItems) {
            objectsListModel->setItem(nextRow, column++, objectEntityItem);
        }

        resizeRowsToContents();
        return nextRow;
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
            const ObjectEntity* selectObjectEntity = getSelectedObjectEntity();
            if (selectObjectEntity == &updatedObjectEntity) {
                std::vector<QStandardItem*> objectEntityItems = buildObjectEntityItems(updatedObjectEntity);
                int column = 0;
                for (QStandardItem* objectEntityItem : objectEntityItems) {
                    objectsListModel->setItem(currentIndex().row(), column++, objectEntityItem);
                }

                resizeRowsToContents();
                return true;
            } else {
                throw std::runtime_error("Update the selected object with a different instance is not allowed");
            }
        }
        return false;
    }

    bool ObjectTableView::moveUpSelectedObject() {
        if (hasObjectEntitySelected() && !isFirstObjectEntitySelected()) {
            int currentRow = this->currentIndex().row();
            int newRow = currentRow - 1;
            QList<QStandardItem*> itemList = objectsListModel->takeRow(currentRow);
            objectsListModel->insertRow(newRow, itemList);
            selectRow(newRow);

            resizeRowsToContents();
            return true;
        }
        return false;
    }

    bool ObjectTableView::moveDownSelectedObject() {
        if (hasObjectEntitySelected() && !isLastObjectEntitySelected()) {
            int currentRow = this->currentIndex().row();
            int newRow = currentRow + 1;
            QList<QStandardItem*> itemList = objectsListModel->takeRow(currentRow);
            objectsListModel->insertRow(newRow, itemList);
            selectRow(newRow);

            resizeRowsToContents();
            return true;
        }
        return false;
    }

    void ObjectTableView::removeAllObjects() {
        objectsListModel->removeRows(0, objectsListModel->rowCount());
        resizeRowsToContents();
    }

}
