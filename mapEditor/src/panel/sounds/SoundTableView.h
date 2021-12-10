#pragma once

#include <QtWidgets/QTableView>
#include <QStandardItemModel>

#include <UrchinCommon.h>
#include <UrchinMapHandler.h>
#include <controller/SceneController.h>

Q_DECLARE_METATYPE(const urchin::SoundEntity*)

namespace urchin {

    class SoundTableView : public QTableView, public Observable {
        Q_OBJECT

        public:
            explicit SoundTableView(QWidget* = nullptr);

            enum NotificationType {
                SOUND_SELECTION_CHANGED
            };

            bool hasSoundEntitySelected() const;
            const SoundEntity* getSelectedSoundEntity() const;

            void addSound(const SoundEntity&);
            bool removeSelectedSound();
            void removeAllSounds();

        private:
            QStandardItemModel* soundsListModel;

            void selectionChanged(const QItemSelection&, const QItemSelection&) override;
    };

}
