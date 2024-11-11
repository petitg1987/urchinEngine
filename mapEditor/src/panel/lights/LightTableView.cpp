#include <QHeaderView>

#include <panel/lights/LightTableView.h>

namespace urchin {

    LightTableView::LightTableView(QWidget* parent) :
            QTableView(parent) {
        lightsListModel = new QStandardItemModel(0, 1, this);
        lightsListModel->setHorizontalHeaderItem(0, new QStandardItem("Light Name"));
        QTableView::setModel(lightsListModel);

        horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
        horizontalHeader()->resizeSection(0, 355);
        verticalHeader()->hide();

        setSelectionMode(SingleSelection);
        setSelectionBehavior(SelectRows);
    }

    void LightTableView::selectionChanged(const QItemSelection&, const QItemSelection&) {
        //hack to refresh selection
        horizontalHeader()->resizeSection(0, 341);
        horizontalHeader()->resizeSection(0, 340);

        notifyObservers(this, LIGHT_SELECTION_CHANGED);
    }

    bool LightTableView::hasLightEntitySelected() const {
        return this->currentIndex().row() != -1;
    }

    const LightEntity* LightTableView::getSelectedLightEntity() const {
        QModelIndex selectedIndex = this->currentIndex();
        if (selectedIndex.row() != -1) {
            return selectedIndex.data(Qt::UserRole + 1).value<const LightEntity*>();
        }
        return nullptr;
    }

    int LightTableView::addLight(const LightEntity& lightEntity) {
        auto* itemLightName = new QStandardItem(QString::fromStdString(lightEntity.getName()));
        itemLightName->setData(QVariant::fromValue(&lightEntity), Qt::UserRole + 1);
        itemLightName->setEditable(false);

        int nextRow = lightsListModel->rowCount();
        lightsListModel->insertRow(nextRow);
        lightsListModel->setItem(nextRow, 0, itemLightName);

        resizeRowsToContents();
        return nextRow;
    }

    bool LightTableView::removeSelectedLight() {
        if (hasLightEntitySelected()) {
            lightsListModel->removeRow(this->currentIndex().row());
            resizeRowsToContents();

            return true;
        }

        return false;
    }

    void LightTableView::removeAllLights() {
        lightsListModel->removeRows(0, lightsListModel->rowCount());
        resizeRowsToContents();
    }

}
