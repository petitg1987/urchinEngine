#include <QHeaderView>

#include <panel/terrains/TerrainTableView.h>

namespace urchin {
    TerrainTableView::TerrainTableView(QWidget* parent) :
            QTableView(parent) {
        terrainsListModel = new QStandardItemModel(0, 2, this);
        terrainsListModel->setHorizontalHeaderItem(0, new QStandardItem("Terrain Name"));
        terrainsListModel->setHorizontalHeaderItem(1, new QStandardItem("Height File"));
        QTableView::setModel(terrainsListModel);

        horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
        horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
        horizontalHeader()->resizeSection(0, 180);
        horizontalHeader()->resizeSection(1, 160);
        verticalHeader()->hide();

        setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    }

    void TerrainTableView::selectionChanged(const QItemSelection&, const QItemSelection&) {
        //hack to refresh selection
        horizontalHeader()->resizeSection(0, 180);
        horizontalHeader()->resizeSection(0, 160);

        notifyObservers(this, NotificationType::SELECTION_CHANGED);
    }

    bool TerrainTableView::hasTerrainEntitySelected() const {
        return this->currentIndex().row() != -1;
    }

    const TerrainEntity* TerrainTableView::getSelectedTerrainEntity() const {
        QModelIndex selectedIndex = this->currentIndex();
        if (selectedIndex.row() != -1) {
            return selectedIndex.data(Qt::UserRole + 1).value<const TerrainEntity*>();
        }
        return nullptr;
    }

    void TerrainTableView::addTerrain(const TerrainEntity& terrainEntity) {
        auto* itemTerrainName = new QStandardItem(QString::fromStdString(terrainEntity.getName()));
        itemTerrainName->setData(QVariant::fromValue(&terrainEntity), Qt::UserRole + 1);
        itemTerrainName->setEditable(false);

        std::string pathFileName = terrainEntity.getTerrain()->getMesh()->getHeightFilename();
        auto* itemHeightFile = new QStandardItem(QString::fromStdString(FileUtil::getFileName(pathFileName)));
        itemHeightFile->setToolTip(QString::fromStdString(pathFileName));
        itemHeightFile->setData(QVariant::fromValue(&terrainEntity), Qt::UserRole + 1);
        itemHeightFile->setEditable(false);

        int nextRow = terrainsListModel->rowCount();
        terrainsListModel->insertRow(nextRow);
        terrainsListModel->setItem(nextRow, 0, itemTerrainName);
        terrainsListModel->setItem(nextRow, 1, itemHeightFile);

        resizeRowsToContents();
    }

    bool TerrainTableView::removeSelectedTerrain() {
        if (hasTerrainEntitySelected()) {
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
