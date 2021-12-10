#include <QHeaderView>
#include <QVariant>

#include <panel/models/ModelTableView.h>

namespace urchin {

    ModelTableView::ModelTableView(QWidget* parent) :
            QTableView(parent) {
        objectsListModel = new QStandardItemModel(0, 2, this);
        objectsListModel->setHorizontalHeaderItem(0, new QStandardItem("Object Name"));
        objectsListModel->setHorizontalHeaderItem(1, new QStandardItem("Meshes File"));
        QTableView::setModel(objectsListModel);

        horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
        horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
        horizontalHeader()->resizeSection(0, 90);
        horizontalHeader()->resizeSection(1, 265);
        verticalHeader()->hide();

        setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    }

    void ModelTableView::selectionChanged(const QItemSelection&, const QItemSelection&) {
        //hack to refresh selection
        horizontalHeader()->resizeSection(0, 91);
        horizontalHeader()->resizeSection(0, 90);

        notifyObservers(this, NotificationType::OBJECT_SELECTION_CHANGED);
    }

    bool ModelTableView::hasObjectEntitySelected() const {
        return this->currentIndex().row() != -1 && !this->selectedIndexes().empty();
    }

    int ModelTableView::getObjectEntityRow(const ObjectEntity* expectedObjectEntity) const {
        for (int rowId = 0; rowId < objectsListModel->rowCount(); ++rowId) {
            QModelIndex index = objectsListModel->index(rowId, 0);
            auto* objectEntity = index.data(Qt::UserRole + 1).value<const ObjectEntity*>();
            if (expectedObjectEntity->getName() == objectEntity->getName()) {
                return rowId;
            }
        }

        return -1;
    }

    const ObjectEntity* ModelTableView::getSelectedObjectEntity() const {
        if (hasObjectEntitySelected()) {
            return this->currentIndex().data(Qt::UserRole + 1).value<const ObjectEntity*>();
        }
        return nullptr;
    }

    int ModelTableView::addObject(const ObjectEntity& objectEntity) {
        auto* itemModelName = new QStandardItem(QString::fromStdString(objectEntity.getName()));
        itemModelName->setData(QVariant::fromValue(&objectEntity), Qt::UserRole + 1);
        itemModelName->setEditable(false);

        std::string meshesName;
        std::string meshesFilename;
        if (objectEntity.getModel()->getConstMeshes()) {
            meshesName = objectEntity.getModel()->getConstMeshes()->getMeshesName();
            meshesFilename = objectEntity.getModel()->getConstMeshes()->getMeshesFilename();
        }
        auto* itemMeshName = new QStandardItem(QString::fromStdString(meshesName));
        itemMeshName->setToolTip(QString::fromStdString(meshesFilename));
        itemMeshName->setData(QVariant::fromValue(&objectEntity), Qt::UserRole + 1);
        itemMeshName->setEditable(false);

        int nextRow = objectsListModel->rowCount();
        objectsListModel->insertRow(nextRow);
        objectsListModel->setItem(nextRow, 0, itemModelName);
        objectsListModel->setItem(nextRow, 1, itemMeshName);

        resizeRowsToContents();

        return nextRow;
    }

    bool ModelTableView::removeSelectedObject() {
        if (hasObjectEntitySelected()) {
            objectsListModel->removeRow(this->currentIndex().row());
            resizeRowsToContents();

            return true;
        }

        return false;
    }

    void ModelTableView::removeAllObjects() {
        objectsListModel->removeRows(0, objectsListModel->rowCount());
        resizeRowsToContents();
    }

}
