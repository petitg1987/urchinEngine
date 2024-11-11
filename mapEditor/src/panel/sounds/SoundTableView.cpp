#include <QHeaderView>
#include <QVariant>
#include <UrchinAggregation.h>

#include <panel/sounds/SoundTableView.h>

namespace urchin {

    SoundTableView::SoundTableView(QWidget* parent) :
            QTableView(parent) {
        soundsListModel = new QStandardItemModel(0, 2, this);
        soundsListModel->setHorizontalHeaderItem(0, new QStandardItem("Sound Name"));
        soundsListModel->setHorizontalHeaderItem(1, new QStandardItem("Sound File"));
        QTableView::setModel(soundsListModel);

        horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
        horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
        horizontalHeader()->resizeSection(0, 180);
        horizontalHeader()->resizeSection(1, 160);
        verticalHeader()->hide();

        setSelectionMode(SingleSelection);
        setSelectionBehavior(SelectRows);
    }

    void SoundTableView::selectionChanged(const QItemSelection&, const QItemSelection&) {
        //hack to refresh selection
        horizontalHeader()->resizeSection(0, 180);
        horizontalHeader()->resizeSection(0, 160);

        notifyObservers(this, SOUND_SELECTION_CHANGED);
    }

    bool SoundTableView::hasSoundEntitySelected() const {
        return this->currentIndex().row() != -1;
    }

    const SoundEntity* SoundTableView::getSelectedSoundEntity() const {
        QModelIndex selectedIndex = this->currentIndex();
        if (selectedIndex.row() != -1) {
            return selectedIndex.data(Qt::UserRole + 1).value<const SoundEntity*>();
        }
        return nullptr;
    }

    int SoundTableView::addSound(const SoundEntity& soundEntity) {
        auto* itemSoundName = new QStandardItem(QString::fromStdString(soundEntity.getName()));
        itemSoundName->setData(QVariant::fromValue(&soundEntity), Qt::UserRole + 1);
        itemSoundName->setEditable(false);

        std::string soundFilename = soundEntity.getSoundComponent()->getSound().getFilename();
        std::string relativeSoundFilename = PathUtil::computeRelativePath(FileSystem::instance().getResourcesDirectory(), soundFilename);
        auto* itemSoundFile = new QStandardItem(QString::fromStdString(FileUtil::getFileName(soundFilename)));
        itemSoundFile->setToolTip(QString::fromStdString(relativeSoundFilename));
        itemSoundFile->setData(QVariant::fromValue(&soundEntity), Qt::UserRole + 1);
        itemSoundFile->setEditable(false);

        int nextRow = soundsListModel->rowCount();
        soundsListModel->insertRow(nextRow);
        soundsListModel->setItem(nextRow, 0, itemSoundName);
        soundsListModel->setItem(nextRow, 1, itemSoundFile);

        resizeRowsToContents();
        return nextRow;
    }

    bool SoundTableView::removeSelectedSound() {
        if (hasSoundEntitySelected()) {
            soundsListModel->removeRow(this->currentIndex().row());
            resizeRowsToContents();

            return true;
        }

        return false;
    }

    void SoundTableView::removeAllSounds() {
        soundsListModel->removeRows(0, soundsListModel->rowCount());
        resizeRowsToContents();
    }

}
