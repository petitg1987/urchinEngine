#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>

#include <UrchinAggregation.h>
#include <controller/sounds/SoundController.h>

namespace urchin {

    class NewSoundDialog : public QDialog {
        Q_OBJECT

        public:
            static constexpr char GLOBAL_SOUND_LABEL[] = "Global";
            static constexpr char SPATIAL_SOUND_LABEL[] = "Spatial";
            static constexpr char MUSIC_SOUND_LABEL[] = "Music";
            static constexpr char EFFECTS_SOUND_LABEL[] = "Effect";

            NewSoundDialog(QWidget*, const SoundController*);

            std::unique_ptr<SoundEntity> moveSoundEntity();

        private:
            void setupNameFields(QGridLayout*);
            void setupSoundFilenameFields(QGridLayout*);
            void setupSoundTypeFields(QGridLayout*);
            void setupSoundCategoryFields(QGridLayout*);
            void setupSoundInitialVolume(QGridLayout*);

            void updateSoundName();
            int buildSoundEntity(int);

            void done(int) override;
            bool isSoundEntityExist(const std::string&);

            const SoundController* soundController;

            QLabel* soundNameLabel;
            QLineEdit* soundNameText;
            QLabel* soundFilenameLabel;
            QLineEdit* soundFilenameText;
            QComboBox* soundTypeComboBox;
            QComboBox* soundCategoryComboBox;
            QDoubleSpinBox* initialVolumeSpinBox;

            std::string soundName;
            std::string soundFilename;
            std::unique_ptr<SoundEntity> soundEntity;
            static QString preferredSoundPath;

        private slots:
            void showSoundFilenameDialog();
    };

}
