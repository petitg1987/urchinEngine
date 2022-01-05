#pragma once

#include <list>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>

#include <UrchinAggregation.h>
#include <UrchinSoundEngine.h>

namespace urchin {

    class ChangeSoundTriggerDialog : public QDialog {
        Q_OBJECT

        public:
            static constexpr char MANUAL_TRIGGER_LABEL[] = "Manual";
            static constexpr char ZONE_TRIGGER_LABEL[] = "Zone";

            explicit ChangeSoundTriggerDialog(QWidget*);

            SoundTrigger::TriggerType getTriggerType() const;

        private:
            void setupSoundTriggerTypeFields(QGridLayout*);

            void done(int) override;

            QLabel* soundTriggerTypeLabel;
            QComboBox* soundTriggerTypeComboBox;

            SoundTrigger::TriggerType triggerType;
    };

}
