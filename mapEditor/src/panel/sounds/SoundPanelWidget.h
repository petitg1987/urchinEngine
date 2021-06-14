#pragma once

#include <string>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>

#include <UrchinCommon.h>
#include <controller/sounds/SoundController.h>
#include <panel/sounds/SoundTableView.h>
#include <panel/sounds/soundshape/SoundShapeWidget.h>

namespace urchin {

    class SoundPanelWidget : public QWidget, public Observer {
        Q_OBJECT

        #define PLAY_ONCE_LABEL "Play once"
        #define PLAY_LOOP_LABEL "Play loop"

        public:
            SoundPanelWidget();
            ~SoundPanelWidget() override = default;

            SoundTableView* getSoundTableView() const;

            void load(SoundController*);
            void unload();

        private:
            void setupSoundGeneralPropertiesBox(QVBoxLayout*);
            void setupSpecificPointSoundBox(QVBoxLayout*);
            void setupSoundTriggerGeneralPropertiesBox(QVBoxLayout*);
            void setupSpecificTriggerShapeBox(QVBoxLayout*);

            void notify(Observable*, int) override;
            void setupSoundDataFrom(const SceneSound*);
            void setupAmbientSoundDataFrom();
            void setupPointSoundDataFrom(const PointSound*);
            void setupPlayBehaviorDataFrom(const SoundTrigger*);
            void setupManualTriggerDataFrom();
            void setupShapeTriggerDataFrom(const SceneSound*);

            SoundShapeWidget* retrieveSoundShapeWidget(const SoundShape*, const SceneSound*);

            SoundController* soundController;

            SoundTableView* soundTableView;
            QPushButton* addSoundButton;
            QPushButton* removeSoundButton;

            QGroupBox* soundPropertiesGroupBox;
            QGroupBox* soundTriggerGroupBox;
            QGroupBox* specificTriggerShapeGroupBox;
            QGroupBox* specificPointSoundGroupBox;

            bool disableSoundEvent;

            //sound general properties
            QDoubleSpinBox* initialVolume;
            QLabel* soundType;

            //sound specific properties
            QDoubleSpinBox* positionX;
            QDoubleSpinBox* positionY;
            QDoubleSpinBox* positionZ;
            QDoubleSpinBox* inaudibleDistance;

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
            void showAddSoundDialog();
            void removeSelectedSound();

            void updateSoundGeneralProperties();
            void updateSoundSpecificProperties();
            void updateSoundTriggerProperties();

            void showChangeSoundTriggerDialog();
            void showChangeSoundShapeDialog();
            void soundShapeChanged(const SoundShape*);
    };

}
