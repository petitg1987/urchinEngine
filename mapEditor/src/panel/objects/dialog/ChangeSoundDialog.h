#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <UrchinSoundEngine.h>

namespace urchin {

    class ChangeSoundDialog final : public QDialog {
        Q_OBJECT

        public:
            static constexpr char NONE_LABEL[] = "None";
            static constexpr char LOCALIZABLE_SOUND_LABEL[] = "Localizable";
            static constexpr char GLOBAL_SOUND_LABEL[] = "Global";

            explicit ChangeSoundDialog(QWidget*);

            std::optional<Sound::SoundType> getSoundType() const;
            std::string getSoundFilename() const;

        private:
            void setupSoundTypeFields(QGridLayout*);
            void setupSoundFilenameFields(QGridLayout*);

            void done(int) override;

            QComboBox* soundTypeComboBox;
            QLabel* soundFilenameLabel;
            QLineEdit* soundFilenameText;

            std::optional<Sound::SoundType> soundType;
            std::string soundFilename;
            static QString preferredSoundPath;

        private slots:
            void showSoundFilenameDialog();
    };

}
