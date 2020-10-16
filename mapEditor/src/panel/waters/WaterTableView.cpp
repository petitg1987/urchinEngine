#include <QHeaderView>

#include "WaterTableView.h"

namespace urchin {
    WaterTableView::WaterTableView(QWidget *parent) :
            QTableView(parent) {
        watersListModel = new QStandardItemModel(0, 1, this);
        watersListModel->setHorizontalHeaderItem(0, new QStandardItem("Water Name"));
        QTableView::setModel(watersListModel);

        horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
        horizontalHeader()->resizeSection(0, 355);
        verticalHeader()->hide();

        setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    }

    void WaterTableView::selectionChanged(const QItemSelection &, const QItemSelection &) {
        //hack to refresh selection
        horizontalHeader()->resizeSection(0, 341);
        horizontalHeader()->resizeSection(0, 340);

        notifyObservers(this, NotificationType::SELECTION_CHANGED);
    }

    bool WaterTableView::hasSceneWaterSelected() const {
        return this->currentIndex().row()!=-1;
    }

    const SceneWater *WaterTableView::getSelectedSceneWater() const {
        QModelIndex selectedIndex = this->currentIndex();
        if(selectedIndex.row()!=-1) {
            return selectedIndex.data(Qt::UserRole + 1).value<const SceneWater *>();
        }
        return nullptr;
    }

    void WaterTableView::addWater(const SceneWater *sceneWater) {
        auto *itemWaterName = new QStandardItem(QString::fromStdString(sceneWater->getName()));
        itemWaterName->setData(qVariantFromValue(sceneWater), Qt::UserRole + 1);
        itemWaterName->setEditable(false);

        int nextRow = watersListModel->rowCount();
        watersListModel->insertRow(nextRow);
        watersListModel->setItem(nextRow, 0, itemWaterName);

        resizeRowsToContents();
    }

    bool WaterTableView::removeSelectedWater() {
        if(hasSceneWaterSelected()) {
            watersListModel->removeRow(this->currentIndex().row());
            resizeRowsToContents();

            return true;
        }

        return false;
    }

    void WaterTableView::removeAllWaters() {
        watersListModel->removeRows(0, watersListModel->rowCount());
        resizeRowsToContents();
    }
}
