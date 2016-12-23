#ifndef ENGINE_MAPEDITOR_SOUNDCONTROLLERWIDGET_H
#define ENGINE_MAPEDITOR_SOUNDCONTROLLERWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>

#include "UrchinCommon.h"
#include "scene/controller/sounds/SoundController.h"
#include "scene/controller/sounds/SoundTableView.h"
#include "scene/controller/sounds/soundshape/SoundShapeWidget.h"

namespace urchin
{

	class SoundControllerWidget : public QWidget, public Observer
	{
		Q_OBJECT

		#define PLAY_ONCE_LABEL "Play once"
		#define PLAY_LOOP_LABEL "Play loop"
		#define INSTANT_STOP_LABEL "Instant Stop"
		#define SMOOTH_STOP_LABEL "Smooth Stop"

		public:
			SoundControllerWidget();
			virtual ~SoundControllerWidget();

			SoundTableView *getSoundTableView() const;

			void load(SoundController *);
			void unload();

			bool isModified() const;

		private:
			void setupSoundGeneralPropertiesBox(QVBoxLayout *);
			void setupSpecificPointSoundBox(QVBoxLayout *);
			void setupSoundBehaviorPropertiesBox(QVBoxLayout *);
			void setupSpecificTriggerShapeBox(QVBoxLayout *);

			void notify(Observable *, int);
			void setupSoundDataFrom(const SceneSound *);
			void setupAmbientSoundDataFrom(const AmbientSound *);
			void setupPointSoundDataFrom(const PointSound *);
			void setupSoundBehaviorDataFrom(const SoundTrigger *);
			void setupManualTriggerDataFrom(const SceneSound *);
			void setupShapeTriggerDataFrom(const SceneSound *);

			SoundShapeWidget *retrieveSoundShapeWidget(const SoundShape *, const SceneSound *);

			SoundController *soundController;

			SoundTableView *soundTableView;
			QPushButton *addSoundButton;
			QPushButton *removeSoundButton;

			QGroupBox *soundPropertiesGroupBox;
			QGroupBox *soundTriggerGroupBox;

			bool disableSoundEvent;

			//sound general properties
			QDoubleSpinBox *volume;
			QLabel *soundType;

			//sound specific properties
			QGroupBox *specificPointSoundGroupBox;
			QDoubleSpinBox *positionX, *positionY, *positionZ, *inaudibleDistance;

			//sound behavior
			QComboBox *playBehavior, *stopBehavior;
			QDoubleSpinBox *volumeDecreasePercentageOnStop;
			QLabel *soundTriggerType;

			//sound trigger specific properties
			QGroupBox *specificTriggerShapeGroupBox;
			QGridLayout *triggerShapeLayout;
			QPushButton *changeSoundTriggerTypeButton;
			QLabel *soundShapeType;
			QPushButton *changeSoundShapeTypeButton;
			SoundShapeWidget *soundShapeWidget;

		private slots:
			void showAddSoundDialog();
			void removeSelectedSound();

			void updateSoundGeneralProperties();
			void updateSoundSpecificProperties();
			void updateSoundBehaviorProperties();

			void showChangeSoundTriggerDialog();
			void showChangeSoundShapeDialog();
			void soundShapeChanged(const SoundShape *);
	};

}

#endif
