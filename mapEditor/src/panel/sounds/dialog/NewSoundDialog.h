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

        #define AMBIENT_SOUND_LABEL "Ambient"
        #define POINT_SOUND_LABEL "Point"

        public:
            NewSoundDialog(QWidget*, const SoundController*);

            SceneSound* getSceneSound() const;

        private:
            void setupNameFields(QGridLayout*);
            void setupSoundFilenameFields(QGridLayout*);
            void setupSoundTypeFields(QGridLayout*);

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

            std::string soundName;
            std::string soundFilename;
            SceneSound* sceneSound;
            static QString preferredSoundPath;

        private slots:
            void showSoundFilenameDialog();
    };

}
