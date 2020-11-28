#include <QHeaderView>
#include <QVariant>

#include "SoundTableView.h"

namespace urchin {

    SoundTableView::SoundTableView(QWidget *parent) :
        QTableView(parent) {
        soundsListModel = new QStandardItemModel(0, 2, this);
        soundsListModel->setHorizontalHeaderItem(0, new QStandardItem("Sound Name"));
        soundsListModel->setHorizontalHeaderItem(1, new QStandardItem("Sound File"));
        QTableView::setModel(soundsListModel);

        horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
        horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
        horizontalHeader()->resizeSection(0, 90);
        horizontalHeader()->resizeSection(1, 265);
        verticalHeader()->hide();

        setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    }

    void SoundTableView::selectionChanged(const QItemSelection &, const QItemSelection &) {
        //hack to refresh selection
        horizontalHeader()->resizeSection(0, 91);
        horizontalHeader()->resizeSection(0, 90);

        notifyObservers(this, NotificationType::SOUND_SELECTION_CHANGED);
    }

    bool SoundTableView::hasSceneSoundSelected() const {
        return this->currentIndex().row()!=-1;
    }

    const SceneSound *SoundTableView::getSelectedSceneSound() const {
        QModelIndex selectedIndex = this->currentIndex();
        if (selectedIndex.row()!=-1) {
            return selectedIndex.data(Qt::UserRole + 1).value<const SceneSound *>();
        }
        return nullptr;
    }

    void SoundTableView::addSound(const SceneSound *sceneSound) {
        auto *itemSoundName = new QStandardItem(QString::fromStdString(sceneSound->getName()));
        itemSoundName->setData(QVariant::fromValue(sceneSound), Qt::UserRole + 1);
        itemSoundName->setEditable(false);

        auto *itemSoundFile = new QStandardItem(QString::fromStdString(sceneSound->getSound()->getFilename()));
        itemSoundFile->setData(QVariant::fromValue(sceneSound), Qt::UserRole + 1);
        itemSoundFile->setEditable(false);

        int nextRow = soundsListModel->rowCount();
        soundsListModel->insertRow(nextRow);
        soundsListModel->setItem(nextRow, 0, itemSoundName);
        soundsListModel->setItem(nextRow, 1, itemSoundFile);

        resizeRowsToContents();
    }

    bool SoundTableView::removeSelectedSound() {
        if (hasSceneSoundSelected()) {
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
