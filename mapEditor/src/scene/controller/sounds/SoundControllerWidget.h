#ifndef ENGINE_MAPEDITOR_SOUNDCONTROLLERWIDGET_H
#define ENGINE_MAPEDITOR_SOUNDCONTROLLERWIDGET_H

#include <string>
#include <memory>
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
			void setupSoundGeneralPropertiesBox(QWidget *);
			void setupSoundSpecificPropertiesBox(QWidget *);
			void setupAmbientSoundBox(QWidget *);
			void setupPointSoundBox(QWidget *);
			void setupSoundBehaviorPropertiesBox(QWidget *);
			void setupSoundTriggerSpecificPropertiesBox(QWidget *);
			void setupManuelTriggerBox(QWidget *);
			void setupShapeTriggerBox(QWidget *);

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

			QGroupBox *soundGroupBox;
			QGroupBox *soundTriggerGroupBox;

			bool disableSoundEvent;

			//sound general properties
			QDoubleSpinBox *volume;

			//sound specific properties
			QLabel *soundType;
			QWidget *ambientSoundWidget;
			QWidget *pointSoundWidget;
			QDoubleSpinBox *positionX, *positionY, *positionZ, *inaudibleDistance;

			//sound behavior
			QComboBox *playBehavior, *stopBehavior;
			QDoubleSpinBox *volumeDecreasePercentageOnStop;

			//sound trigger specific properties
			QLabel *soundTriggerType;
			QPushButton *changeSoundTriggerTypeButton;
			QWidget *manualTriggerWidget;
			QWidget *shapeTriggerWidget;
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
