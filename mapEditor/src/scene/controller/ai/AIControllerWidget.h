#ifndef URCHINENGINE_MAPEDITOR_AICONTROLLERWIDGET_H
#define URCHINENGINE_MAPEDITOR_AICONTROLLERWIDGET_H

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

			void setupGenerateNavMeshDataFrom(const std::shared_ptr<NavMeshConfig> &);

			AIController *aiController;

			QDoubleSpinBox *agentHeight;
			QDoubleSpinBox *agentRadius;
			QDoubleSpinBox *maxSlope;

			bool disableAIEvent;

		private slots:
			void navMeshConfigChanged();
	};

}

#endif
