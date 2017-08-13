#ifndef ENGINE_MAPEDITOR_AICONTROLLERWIDGET_H
#define ENGINE_MAPEDITOR_AICONTROLLERWIDGET_H

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>

#include "scene/controller/ai/AIController.h"

namespace urchin
{

	class AIControllerWidget : public QWidget
	{
		Q_OBJECT

		public:
			AIControllerWidget();
			virtual ~AIControllerWidget() = default;

			void load(AIController *);
			void unload();

		private:
			void setupGenerateNavMeshBox(QVBoxLayout *);

			AIController *aiController;

			QDoubleSpinBox *agentHeight;
			QDoubleSpinBox *agentRadius;
			QDoubleSpinBox *maxSlope;

		private slots:
			void navMeshConfigChanged();
	};

}

#endif
