#pragma once

#include <QtWidgets/QGroupBox>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

#include <controller/objects/ObjectController.h>
#include <panel/objects/sound/soundshape/SoundShapeWidget.h>

namespace urchin {

    class SoundWidget final : public QWidget {
        Q_OBJECT

        public:
            static constexpr char PLAY_ONCE_LABEL[] = "Play once";
            static constexpr char PLAY_LOOP_LABEL[] = "Play loop";

            static constexpr char MUSIC_SOUND_LABEL[] = "Music";
            static constexpr char EFFECTS_SOUND_LABEL[] = "Effect";

            static constexpr char AUTO_TRIGGER_LABEL[] = "Auto";
            static constexpr char MANUAL_TRIGGER_LABEL[] = "Manual";
            static constexpr char AREA_TRIGGER_LABEL[] = "Area";

            SoundWidget();

            void load(const ObjectEntity&, ObjectController&);

        private:
            void setupSoundGeneralPropertiesBox(QVBoxLayout*);
            void setupSpecificLocalizableSoundBox(QVBoxLayout*);
            void setupSoundTriggerGeneralPropertiesBox(QVBoxLayout*);
            void setupSpecificAreaTriggerBox(QVBoxLayout*);

            void setupSoundDataFrom();
            void setupGlobalSoundDataFrom() const;
            void setupLocalizableSoundDataFrom(const LocalizableSound&) const;
            void setupPlayBehaviorDataFrom(const SoundTrigger&) const;
            void setupAutoTriggerDataFrom() const;
            void setupManualTriggerDataFrom() const;
            void setupShapeTriggerDataFrom();

            SoundShapeWidget& retrieveSoundShapeWidget(const SoundShape&);

            const ObjectEntity* objectEntity;
            ObjectController* objectController;

            QGroupBox* specificAreaTriggerGroupBox;
            QGroupBox* specificLocalizableSoundGroupBox;

            bool disableSoundEvent;

            //sound general properties
            QLabel* soundFile;
            QComboBox* soundCategory;
            QDoubleSpinBox* initialVolume;

            //sound specific properties
            QDoubleSpinBox* positionX;
            QDoubleSpinBox* positionY;
            QDoubleSpinBox* positionZ;
            QDoubleSpinBox* radius;

            //sound trigger
            QComboBox* playBehavior;
            QLabel* soundTriggerType;

            //sound trigger specific properties
            QVBoxLayout* triggerShapeLayout;
            QPushButton* changeSoundTriggerTypeButton;
            QLabel* soundShapeType;
            QPushButton* changeSoundShapeTypeButton;
            SoundShapeWidget* soundShapeWidget;

        private slots:
            void updateSoundComponent() const;
            void updateSoundSpecificProperties() const;
            void updateSoundTriggerProperties() const;

            void showChangeSoundTriggerDialog();
            void showChangeSoundShapeDialog();
            void soundShapeChanged(SoundShape*) const;
    };

}
