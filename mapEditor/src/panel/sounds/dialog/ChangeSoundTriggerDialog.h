#pragma once

#include <list>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>

#include <UrchinMapHandler.h>
#include <UrchinSoundEngine.h>

namespace urchin {

    class ChangeSoundTriggerDialog : public QDialog {
        Q_OBJECT

        #define MANUAL_TRIGGER_LABEL "Manual"
        #define SHAPE_TRIGGER_LABEL "Shape"

        public:
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
