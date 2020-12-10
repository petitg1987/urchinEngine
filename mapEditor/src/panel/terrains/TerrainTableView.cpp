#include <QHeaderView>

#include "TerrainTableView.h"

namespace urchin {
    TerrainTableView::TerrainTableView(QWidget* parent) :
            QTableView(parent) {
        terrainsListModel = new QStandardItemModel(0, 2, this);
        terrainsListModel->setHorizontalHeaderItem(0, new QStandardItem("Terrain Name"));
        terrainsListModel->setHorizontalHeaderItem(1, new QStandardItem("Height File"));
        QTableView::setModel(terrainsListModel);

        horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
        horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
        horizontalHeader()->resizeSection(0, 90);
        horizontalHeader()->resizeSection(1, 265);
        verticalHeader()->hide();

        setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    }

    void TerrainTableView::selectionChanged(const QItemSelection&, const QItemSelection&) {
        //hack to refresh selection
        horizontalHeader()->resizeSection(0, 91);
        horizontalHeader()->resizeSection(0, 90);

        notifyObservers(this, NotificationType::SELECTION_CHANGED);
    }

    bool TerrainTableView::hasSceneTerrainSelected() const {
        return this->currentIndex().row() != -1;
    }

    const SceneTerrain* TerrainTableView::getSelectedSceneTerrain() const {
        QModelIndex selectedIndex = this->currentIndex();
        if (selectedIndex.row() != -1) {
            return selectedIndex.data(Qt::UserRole + 1).value<const SceneTerrain*>();
        }
        return nullptr;
    }

    void TerrainTableView::addTerrain(const SceneTerrain* sceneTerrain) {
        auto* itemTerrainName = new QStandardItem(QString::fromStdString(sceneTerrain->getName()));
        itemTerrainName->setData(QVariant::fromValue(sceneTerrain), Qt::UserRole + 1);
        itemTerrainName->setEditable(false);

        std::string pathFileName = sceneTerrain->getTerrain()->getMesh()->getHeightFilename();
        auto* itemHeightFile = new QStandardItem(QString::fromStdString(FileHandler::getFileName(pathFileName)));
        itemHeightFile->setToolTip(QString::fromStdString(pathFileName));
        itemHeightFile->setData(QVariant::fromValue(sceneTerrain), Qt::UserRole + 1);
        itemHeightFile->setEditable(false);

        int nextRow = terrainsListModel->rowCount();
        terrainsListModel->insertRow(nextRow);
        terrainsListModel->setItem(nextRow, 0, itemTerrainName);
        terrainsListModel->setItem(nextRow, 1, itemHeightFile);

        resizeRowsToContents();
    }

    bool TerrainTableView::removeSelectedTerrain() {
        if (hasSceneTerrainSelected()) {
            terrainsListModel->removeRow(this->currentIndex().row());
            resizeRowsToContents();

            return true;
        }

        return false;
    }

    void TerrainTableView::removeAllTerrains() {
        terrainsListModel->removeRows(0, terrainsListModel->rowCount());
        resizeRowsToContents();
    }
}
