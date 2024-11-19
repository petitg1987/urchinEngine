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

    class NewSoundDialog final : public QDialog {
        Q_OBJECT

        public:
            static constexpr char GLOBAL_SOUND_LABEL[] = "Global";
            static constexpr char LOCALIZABLE_SOUND_LABEL[] = "Localizable";
            static constexpr char MUSIC_SOUND_LABEL[] = "Music";
            static constexpr char EFFECTS_SOUND_LABEL[] = "Effect";
            static constexpr char AUTO_TRIGGER_LABEL[] = "Auto";
            static constexpr char MANUAL_TRIGGER_LABEL[] = "Manual";
            static constexpr char AREA_TRIGGER_LABEL[] = "Area";

            NewSoundDialog(QWidget*, const SoundController*);

            std::unique_ptr<SoundEntity> moveSoundEntity();

        private:
            void setupNameFields(QGridLayout*);
            void setupSoundFilenameFields(QGridLayout*);
            void setupSoundTypeFields(QGridLayout*);
            void setupSoundCategoryFields(QGridLayout*);
            void setupSoundInitialVolume(QGridLayout*);
            void setupSoundTriggerTypeFields(QGridLayout*);

            void updateSoundName();
            int buildSoundEntity(int);

            void done(int) override;
            bool isSoundEntityExist(std::string_view) const;

            const SoundController* soundController;

            QLabel* soundNameLabel;
            QLineEdit* soundNameText;
            QLabel* soundFilenameLabel;
            QLineEdit* soundFilenameText;
            QComboBox* soundTypeComboBox;
            QComboBox* soundCategoryComboBox;
            QDoubleSpinBox* initialVolumeSpinBox;
            QComboBox* soundTriggerTypeComboBox;

            std::string soundName;
            std::string soundFilename;
            std::unique_ptr<SoundEntity> soundEntity;
            static QString preferredSoundPath;

        private slots:
            void showSoundFilenameDialog();
    };

}
