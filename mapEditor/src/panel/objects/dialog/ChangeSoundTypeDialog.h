#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <UrchinSoundEngine.h>

namespace urchin {

    class ChangeSoundTypeDialog final : public QDialog {
        Q_OBJECT

        public:
            static constexpr char NONE_LABEL[] = "None";
            static constexpr char LOCALIZABLE_SOUND_LABEL[] = "Localizable";
            static constexpr char GLOBAL_SOUND_LABEL[] = "Global";

            explicit ChangeSoundTypeDialog(QWidget*);

            std::optional<Sound::SoundType> getSoundType() const;

        private:
            void setupSoundTypeFields(QGridLayout*);

            void done(int) override;

            QComboBox* soundTypeComboBox;

            std::optional<Sound::SoundType> soundType;
    };

}
