#ifndef URCHINENGINE_MAPEDITOR_NEWLIGHTDIALOG_H
#define URCHINENGINE_MAPEDITOR_NEWLIGHTDIALOG_H

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>

#include "UrchinMapHandler.h"
#include "scene/controller/lights/LightController.h"

namespace urchin
{

	class NewLightDialog : public QDialog
	{
		Q_OBJECT

		#define SUN_LIGHT_LABEL "Sun"
		#define OMNIDIRECTIONAL_LIGHT_LABEL "Omnidirectional"

		public:
			NewLightDialog(QWidget *parent, const LightController *);

			SceneLight *getSceneLight() const;

		private:
			void setupNameFields(QGridLayout *);
			void setupLightTypeFields(QGridLayout *);

			void updateLightName();
			int buildSceneLight(int);

			void done(int);
			bool isSceneLightExist(const std::string &);

			const LightController *lightController;

			QLabel *lightNameLabel;
			QLineEdit *lightNameText;
			QLabel *lightTypeLabel;
			QComboBox *lightTypeComboBox;

			std::string lightName;
			SceneLight *sceneLight;
	};

}

#endif
