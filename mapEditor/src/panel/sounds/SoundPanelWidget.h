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

        public:
            static constexpr char PLAY_ONCE_LABEL[] = "Play once";
            static constexpr char PLAY_LOOP_LABEL[] = "Play loop";

            SoundPanelWidget();
            ~SoundPanelWidget() override = default;

            SoundTableView* getSoundTableView() const;

            void load(SoundController&);
            void unload();

        private:
            void setupSoundGeneralPropertiesBox(QVBoxLayout*);
            void setupSpecificSpatialSoundBox(QVBoxLayout*);
            void setupSoundTriggerGeneralPropertiesBox(QVBoxLayout*);
            void setupSpecificZoneTriggerBox(QVBoxLayout*);

            void notify(Observable*, int) override;
            void setupSoundDataFrom(const SoundEntity&);
            void setupGlobalSoundDataFrom();
            void setupSpatialSoundDataFrom(const SpatialSound&);
            void setupPlayBehaviorDataFrom(const SoundTrigger&);
            void setupManualTriggerDataFrom();
            void setupShapeTriggerDataFrom(const SoundEntity&);

            SoundShapeWidget& retrieveSoundShapeWidget(const SoundShape&, const SoundEntity&);

            SoundController* soundController;

            SoundTableView* soundTableView;
            QPushButton* addSoundButton;
            QPushButton* removeSoundButton;

            QGroupBox* soundPropertiesGroupBox;
            QGroupBox* soundTriggerGroupBox;
            QGroupBox* specificZoneTriggerGroupBox;
            QGroupBox* specificSpatialSoundGroupBox;

            bool disableSoundEvent;

            //sound general properties
            QLabel* soundType;
            QLabel* soundCategory;
            QLabel* initialVolume;

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
            QLabel* soundShapeType;
            QPushButton* changeSoundShapeTypeButton;
            std::unique_ptr<SoundShapeWidget> soundShapeWidget;

        private slots:
            void showAddSoundDialog();
            void removeSelectedSound();

            void updateSoundSpecificProperties();
            void updateSoundTriggerProperties();

            void showChangeSoundShapeDialog();
            void soundShapeChanged(const SoundShape*);
    };

}
