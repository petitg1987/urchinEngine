#pragma once

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QComboBox>

#include <UrchinSoundEngine.h>

namespace urchin {

    class ChangeSoundTriggerTypeDialog final : public QDialog {
        Q_OBJECT

        public:
            static constexpr char AUTO_TRIGGER_LABEL[] = "Auto";
            static constexpr char MANUAL_TRIGGER_LABEL[] = "Manual";
            static constexpr char AREA_TRIGGER_LABEL[] = "Area";

            explicit ChangeSoundTriggerTypeDialog(QWidget*);

            SoundTrigger::TriggerType getSoundTriggerType() const;

        private:
            void setupSoundTriggerTypeFields(QGridLayout*);

            void done(int) override;

            QComboBox* soundTriggerTypeComboBox;

            SoundTrigger::TriggerType triggerType;
            
    };

}
