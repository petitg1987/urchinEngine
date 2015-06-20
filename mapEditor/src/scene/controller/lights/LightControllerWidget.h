#ifndef ENGINE_MAPEDITOR_LIGHTCONTROLLERWIDGET_H
#define ENGINE_MAPEDITOR_LIGHTCONTROLLERWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>

#include "UrchinCommon.h"
#include "scene/controller/lights/LightController.h"
#include "scene/controller/lights/LightTableView.h"

namespace urchin
{

	class LightControllerWidget : public QWidget, public Observer
	{
		Q_OBJECT

		public:
			LightControllerWidget();
			virtual ~LightControllerWidget();

			LightTableView *getLightTableView() const;

			void load(LightController *);
			void unload();

			bool isModified() const;

		private:
			void setupGeneralPropertiesBox(QWidget *);
			void setupSpecificPropertiesBox(QWidget *);
			void setupOmnidirectionalLightBox(QWidget *);
			void setupSunLightBox(QWidget *);

			void notify(Observable *, int);
			void setupLightDataFrom(const SceneLight *);
			void setupOmnidirectionalLightDataFrom(const OmnidirectionalLight *);
			void setupSunLightDataFrom(const SunLight *);

			LightController *lightController;

			LightTableView *lightTableView;
			QPushButton *addLightButton;
			QPushButton *removeLightButton;

			QGroupBox *lightGroupBox;

			bool disableLightEvent;

			//general properties
			QDoubleSpinBox *ambientR, *ambientG, *ambientB;
			QCheckBox *produceShadowCheckBox;

			//specific properties
			QLabel *lightType;
			QWidget *omnidirectionalLightWidget;
			QDoubleSpinBox *positionX, *positionY, *positionZ, *attenuation;
			QWidget *sunLightWidget;
			QDoubleSpinBox *directionX, *directionY, *directionZ;

		private slots:
			void showAddLightDialog();
			void removeSelectedLight();

			void updateLightGeneralProperties();
			void updateLightSpecificProperties();
	};

}

#endif
