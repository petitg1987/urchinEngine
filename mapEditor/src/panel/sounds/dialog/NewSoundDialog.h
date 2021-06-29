#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>

#include <UrchinMapHandler.h>
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

            SceneSound* getSceneSound() const;

        private:
            void setupNameFields(QGridLayout*);
            void setupSoundFilenameFields(QGridLayout*);
            void setupSoundTypeFields(QGridLayout*);
            void setupSoundCategoryFields(QGridLayout*);

            void updateSoundName();
            int buildSceneSound(int);

            void done(int) override;
            bool isSceneSoundExist(const std::string&);

            const SoundController* soundController;

            QLabel* soundNameLabel;
            QLineEdit* soundNameText;
            QLabel* soundFilenameLabel;
            QLineEdit* soundFilenameText;
            QLabel* soundTypeLabel;
            QComboBox* soundTypeComboBox;
            QLabel* soundCategoryLabel;
            QComboBox* soundCategoryComboBox;

            std::string soundName;
            std::string soundFilename;
            SceneSound* sceneSound;
            static QString preferredSoundPath;

        private slots:
            void showSoundFilenameDialog();
    };

}
