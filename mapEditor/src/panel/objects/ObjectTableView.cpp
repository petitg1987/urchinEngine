#include <QHeaderView>
#include <QVariant>

#include <panel/objects/ObjectTableView.h>

namespace urchin {

    ObjectTableView::ObjectTableView(QWidget* parent) :
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

    void ObjectTableView::selectionChanged(const QItemSelection&, const QItemSelection&) {
        //hack to refresh selection
        horizontalHeader()->resizeSection(0, 91);
        horizontalHeader()->resizeSection(0, 90);

        notifyObservers(this, NotificationType::OBJECT_SELECTION_CHANGED);
    }

    bool ObjectTableView::hasSceneObjectSelected() const {
        return this->currentIndex().row() != -1 && !this->selectedIndexes().empty();
    }

    int ObjectTableView::getSceneObjectRow(const SceneObject* expectedSceneObject) const {
        for (int rowId = 0; rowId < objectsListModel->rowCount(); ++rowId) {
            QModelIndex index = objectsListModel->index(rowId, 0);
            auto* sceneObject = index.data(Qt::UserRole + 1).value<const SceneObject*>();
            if (expectedSceneObject->getName() == sceneObject->getName()) {
                return rowId;
            }
        }

        return -1;
    }

    const SceneObject* ObjectTableView::getSelectedSceneObject() const {
        if (hasSceneObjectSelected()) {
            return this->currentIndex().data(Qt::UserRole + 1).value<const SceneObject*>();
        }
        return nullptr;
    }

    int ObjectTableView::addObject(const SceneObject& sceneObject) {
        auto* itemObjectName = new QStandardItem(QString::fromStdString(sceneObject.getName()));
        itemObjectName->setData(QVariant::fromValue(&sceneObject), Qt::UserRole + 1);
        itemObjectName->setEditable(false);

        std::string meshesName;
        std::string meshesFilename;
        if (sceneObject.getModel()->getConstMeshes()) {
            meshesName = sceneObject.getModel()->getConstMeshes()->getMeshesName();
            meshesFilename = sceneObject.getModel()->getConstMeshes()->getMeshesFilename();
        }
        auto* itemMeshName = new QStandardItem(QString::fromStdString(meshesName));
        itemMeshName->setToolTip(QString::fromStdString(meshesFilename));
        itemMeshName->setData(QVariant::fromValue(&sceneObject), Qt::UserRole + 1);
        itemMeshName->setEditable(false);

        int nextRow = objectsListModel->rowCount();
        objectsListModel->insertRow(nextRow);
        objectsListModel->setItem(nextRow, 0, itemObjectName);
        objectsListModel->setItem(nextRow, 1, itemMeshName);

        resizeRowsToContents();

        return nextRow;
    }

    bool ObjectTableView::removeSelectedObject() {
        if (hasSceneObjectSelected()) {
            objectsListModel->removeRow(this->currentIndex().row());
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
