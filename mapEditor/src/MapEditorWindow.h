#ifndef URCHINENGINE_MAPEDITOR_MAPEDITORWINDOW_H
#define URCHINENGINE_MAPEDITOR_MAPEDITORWINDOW_H

#include <string>
#include <map>
#include <QMainWindow>
#include <QtWidgets/QHBoxLayout>

#include "scene/SceneDisplayerWidget.h"
#include "panel/SceneControllerWidget.h"

namespace urchin
{

	class MapEditorWindow : public QMainWindow, public Observer
	{
		Q_OBJECT

		public:
			explicit MapEditorWindow(std::string );
            ~MapEditorWindow() override;

		protected:
			void closeEvent(QCloseEvent *event) override;

		private:
			void setupMenu();
			void setupSceneDisplayerWidget(QWidget *, QHBoxLayout *);
			void setupSceneControllerWidget(QWidget *, QHBoxLayout *);

            QString getPreferredMapPath();
            void savePreferredMapPath(const std::string &);

			bool checkCurrentMapSaved();
			void updateInterfaceState(bool);
			void updateMapFilename(const QString&);
            void refreshWindowTitle(bool);

			void notify(Observable *, int) override;
			void handleCompoundShapeSelectionChange(Observable *, int);

			static const std::string WINDOW_TITLE;

			StatusBarController statusBarController;

			QAction *saveAction;
			QAction *saveAsAction;
			QAction *closeAction;
			std::map<SceneDisplayer::ViewProperties, QAction *> viewActions;

			std::string mapEditorPath;
			std::string mapFilename;

			SceneController *sceneController;
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
