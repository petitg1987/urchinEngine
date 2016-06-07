#ifndef ENGINE_DEBUGVISUALIZER_DEBUGVISUALIZERWINDOW_H
#define ENGINE_DEBUGVISUALIZER_DEBUGVISUALIZERWINDOW_H

#include <string>
#include <QMainWindow>
#include <QtWidgets/QHBoxLayout>

#include "scene/displayer/SceneDisplayerWidget.h"

namespace urchin
{

	class DebugVisualizerWindow : public QMainWindow
	{
		Q_OBJECT

		#define WINDOW_TITLE "Urchin - Debug Visualizer"

		public:
			DebugVisualizerWindow(const std::string &);
			virtual ~DebugVisualizerWindow();

		private:
			void setupMenu();
			void setupStatusBar();
			void setupSceneDisplayerWidget(QWidget *, QHBoxLayout *);

			QAction *actionNextStepAction;

			std::string debugVisualizerPath;
			std::string debugFilename;
			QString preferredDebugVisualizerPath;

			SceneDisplayerWidget *sceneDisplayerWidget;

		private slots:
			void showOpenDialog();
			void executeExitAction();

			void executeNextStepAction();
	};

}

#endif
