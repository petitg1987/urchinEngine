#ifndef ENGINE_MAPEDITOR_MAPEDITORWINDOW_H
#define ENGINE_MAPEDITOR_MAPEDITORWINDOW_H

#include <string>
#include <vector>
#include <QMainWindow>
#include <QtWidgets/QHBoxLayout>

#include "scene/displayer/SceneDisplayerWidget.h"
#include "scene/controller/SceneControllerWidget.h"

namespace urchin
{

	class MapEditorWindow : public QMainWindow, public Observer
	{
		Q_OBJECT

		#define WINDOW_TITLE "Urchin - Map Editor"

		public:
			MapEditorWindow(const std::string &);
			virtual ~MapEditorWindow();

		protected:
			void closeEvent(QCloseEvent *event);

		private:
			void setupMenu();
			void setupStatusBar();
			void setupSceneDisplayerWidget(QWidget *, QHBoxLayout *);
			void setupSceneControllerWidget(QWidget *, QHBoxLayout *);

			bool checkCurrentMapSaved();
			void updateMenuStatus();
			void updateMapFilename(QString);

			void notify(Observable *, int);
			void handleCompoundShapeSelectionChange(Observable *, int);

			QAction *saveAction;
			QAction *saveAsAction;
			QAction *closeAction;
			std::vector<QAction *> viewActions;

			std::string mapEditorPath;
			std::string mapFilename;
			QString preferredMapPath;

			SceneDisplayerWidget *sceneDisplayerWidget;
			SceneControllerWidget *sceneControllerWidget;

		private slots:
			void showNewDialog();
			void showOpenDialog();
			void executeSaveAction();
			void showSaveAsDialog();
			bool executeCloseAction();
			void executeExitAction();

			void executeViewPropertiesChangeAction();
			SceneControllerWidget::TabName getConcernedTabFor(SceneDisplayer::ViewProperties);
	};

}

#endif
