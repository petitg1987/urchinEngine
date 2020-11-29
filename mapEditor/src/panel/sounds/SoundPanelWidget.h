#ifndef URCHINENGINE_SOUNDPANELWIDGET_H
#define URCHINENGINE_SOUNDPANELWIDGET_H

#include <string>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>

#include "UrchinCommon.h"
#include "controller/sounds/SoundController.h"
#include "panel/sounds/SoundTableView.h"
#include "panel/sounds/soundshape/SoundShapeWidget.h"

namespace urchin {

    class SoundPanelWidget : public QWidget, public Observer {
        Q_OBJECT

        #define PLAY_ONCE_LABEL "Play once"
        #define PLAY_LOOP_LABEL "Play loop"
        #define INSTANT_STOP_LABEL "Instant Stop"
        #define SMOOTH_STOP_LABEL "Smooth Stop"

        public:
            SoundPanelWidget();
            ~SoundPanelWidget() override = default;

            SoundTableView *getSoundTableView() const;

            void load(SoundController*);
            void unload();

        private:
            void setupSoundGeneralPropertiesBox(QVBoxLayout*);
            void setupSpecificPointSoundBox(QVBoxLayout*);
            void setupSoundBehaviorPropertiesBox(QVBoxLayout*);
            void setupSpecificTriggerShapeBox(QVBoxLayout*);

            void notify(Observable*, int) override;
            void setupSoundDataFrom(const SceneSound*);
            void setupAmbientSoundDataFrom();
            void setupPointSoundDataFrom(const PointSound*);
            void setupSoundBehaviorDataFrom(const SoundTrigger*);
            void setupManualTriggerDataFrom();
            void setupShapeTriggerDataFrom(const SceneSound*);

            SoundShapeWidget *retrieveSoundShapeWidget(const SoundShape*, const SceneSound*);

            SoundController *soundController;

            SoundTableView *soundTableView;
            QPushButton *addSoundButton;
            QPushButton *removeSoundButton;

            QGroupBox *soundPropertiesGroupBox;
            QGroupBox *soundTriggerGroupBox;
            QGroupBox *specificTriggerShapeGroupBox;
            QGroupBox *specificPointSoundGroupBox;

            bool disableSoundEvent;

            //sound general properties
            QDoubleSpinBox* volume;
            QLabel* soundType;

            //sound specific properties
            QDoubleSpinBox* positionX;
            QDoubleSpinBox* positionY;
            QDoubleSpinBox* positionZ;
            QDoubleSpinBox* inaudibleDistance;

            //sound behavior
            QComboBox* playBehavior;
            QComboBox* stopBehavior;
            QDoubleSpinBox* volumeDecreasePercentageOnStop;
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
            void updateSoundBehaviorProperties();

            void showChangeSoundTriggerDialog();
            void showChangeSoundShapeDialog();
            void soundShapeChanged(const SoundShape*);
    };

}

#endif
