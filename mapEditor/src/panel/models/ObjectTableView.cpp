#include <QHeaderView>
#include <QVariant>

#include <panel/models/ObjectTableView.h>

namespace urchin {

    ObjectTableView::ObjectTableView(QWidget* parent) :
            QTableView(parent) {
        modelsListModel = new QStandardItemModel(0, 2, this);
        modelsListModel->setHorizontalHeaderItem(0, new QStandardItem("Model Name"));
        modelsListModel->setHorizontalHeaderItem(1, new QStandardItem("Meshes File"));
        QTableView::setModel(modelsListModel);

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

        notifyObservers(this, NotificationType::MODEL_SELECTION_CHANGED);
    }

    bool ObjectTableView::hasSceneModelSelected() const {
        return this->currentIndex().row() != -1 && !this->selectedIndexes().empty();
    }

    int ObjectTableView::getSceneModelRow(const SceneModel* expectedSceneModel) const {
        for (int rowId = 0; rowId < modelsListModel->rowCount(); ++rowId) {
            QModelIndex index = modelsListModel->index(rowId, 0);
            auto* sceneModel = index.data(Qt::UserRole + 1).value<const SceneModel*>();
            if (expectedSceneModel->getName() == sceneModel->getName()) {
                return rowId;
            }
        }

        return -1;
    }

    const SceneModel* ObjectTableView::getSelectedSceneModel() const {
        if (hasSceneModelSelected()) {
            return this->currentIndex().data(Qt::UserRole + 1).value<const SceneModel*>();
        }
        return nullptr;
    }

    int ObjectTableView::addModel(const SceneModel& sceneModel) {
        auto* itemModelName = new QStandardItem(QString::fromStdString(sceneModel.getName()));
        itemModelName->setData(QVariant::fromValue(&sceneModel), Qt::UserRole + 1);
        itemModelName->setEditable(false);

        std::string meshesName;
        std::string meshesFilename;
        if (sceneModel.getModel()->getConstMeshes()) {
            meshesName = sceneModel.getModel()->getConstMeshes()->getMeshesName();
            meshesFilename = sceneModel.getModel()->getConstMeshes()->getMeshesFilename();
        }
        auto* itemMeshName = new QStandardItem(QString::fromStdString(meshesName));
        itemMeshName->setToolTip(QString::fromStdString(meshesFilename));
        itemMeshName->setData(QVariant::fromValue(&sceneModel), Qt::UserRole + 1);
        itemMeshName->setEditable(false);

        int nextRow = modelsListModel->rowCount();
        modelsListModel->insertRow(nextRow);
        modelsListModel->setItem(nextRow, 0, itemModelName);
        modelsListModel->setItem(nextRow, 1, itemMeshName);

        resizeRowsToContents();

        return nextRow;
    }

    bool ObjectTableView::removeSelectedModel() {
        if (hasSceneModelSelected()) {
            modelsListModel->removeRow(this->currentIndex().row());
            resizeRowsToContents();

            return true;
        }

        return false;
    }

    void ObjectTableView::removeAllModels() {
        modelsListModel->removeRows(0, modelsListModel->rowCount());
        resizeRowsToContents();
    }

}
