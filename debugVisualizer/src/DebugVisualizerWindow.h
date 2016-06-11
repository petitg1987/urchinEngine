#ifndef ENGINE_DEBUGVISUALIZER_DEBUGVISUALIZERWINDOW_H
#define ENGINE_DEBUGVISUALIZER_DEBUGVISUALIZERWINDOW_H

#include <string>
#include <QMainWindow>
#include <QtWidgets/QHBoxLayout>

#include "scene/displayer/SceneDisplayerWidget.h"
#include "scene/controller/SceneController.h"

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

			void updateMenuStatus();
			void updateDebugVisualizerFilename(QString);

			QAction *actionNextStepAction;
			QAction *actionReverseStepAction;

			std::string debugVisualizerPath;
			std::string debugFilename;
			QString preferredDebugVisualizerPath;

			SceneDisplayerWidget *sceneDisplayerWidget;
			SceneController *sceneController;

		private slots:
			void showOpenDialog();
			void executeExitAction();

			void executeNextAction();
			void reverseLastAction();
	};

}

#endif
