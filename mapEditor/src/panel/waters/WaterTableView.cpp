#include <QHeaderView>

#include <panel/waters/WaterTableView.h>

namespace urchin {
    WaterTableView::WaterTableView(QWidget* parent) :
            QTableView(parent) {
        watersListModel = new QStandardItemModel(0, 1, this);
        watersListModel->setHorizontalHeaderItem(0, new QStandardItem("Water Name"));
        QTableView::setModel(watersListModel);

        horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
        horizontalHeader()->resizeSection(0, 355);
        verticalHeader()->hide();

        setSelectionMode(SingleSelection);
        setSelectionBehavior(SelectRows);
    }

    void WaterTableView::selectionChanged(const QItemSelection&, const QItemSelection&) {
        //hack to refresh selection
        horizontalHeader()->resizeSection(0, 341);
        horizontalHeader()->resizeSection(0, 340);

        notifyObservers(this, SELECTION_CHANGED);
    }

    bool WaterTableView::hasWaterEntitySelected() const {
        return this->currentIndex().row() != -1;
    }

    const WaterEntity* WaterTableView::getSelectedWaterEntity() const {
        QModelIndex selectedIndex = this->currentIndex();
        if (selectedIndex.row() != -1) {
            return selectedIndex.data(Qt::UserRole + 1).value<const WaterEntity*>();
        }
        return nullptr;
    }

    int WaterTableView::addWater(const WaterEntity& waterEntity) {
        auto* itemWaterName = new QStandardItem(QString::fromStdString(waterEntity.getName()));
        itemWaterName->setData(QVariant::fromValue(&waterEntity), Qt::UserRole + 1);
        itemWaterName->setEditable(false);

        int nextRow = watersListModel->rowCount();
        watersListModel->insertRow(nextRow);
        watersListModel->setItem(nextRow, 0, itemWaterName);

        resizeRowsToContents();
        return nextRow;
    }

    bool WaterTableView::removeSelectedWater() {
        if (hasWaterEntitySelected()) {
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
