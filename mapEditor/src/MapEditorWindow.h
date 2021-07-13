#pragma once

#include <string>
#include <map>
#include <QMainWindow>
#include <QtWidgets/QHBoxLayout>

#include <scene/SceneDisplayer.h>
#include <scene/SceneDisplayerWindow.h>
#include <panel/ScenePanelWidget.h>

namespace urchin {

    class MapEditorWindow : public QMainWindow, public Observer {
        Q_OBJECT

        public:
            explicit MapEditorWindow(std::string);

        protected:
            void closeEvent(QCloseEvent*) override;

        private:
            QString getBaseWindowTitle() const;
            void setupMenu();
            void setupSceneDisplayerWidget(QWidget*, QHBoxLayout*);
            void setupSceneControllerWidget(QWidget*, QHBoxLayout*);

            QString getPreferredMapPath() const;
            void savePreferredMapPath(const std::string&) const;

            void loadMap(const std::string&, const std::string&);
            bool checkCurrentMapSaved();
            void updateInterfaceState();
            void updateMapFilename(const std::string&);
            void refreshWindowTitle();

            void notify(Observable*, int) override;
            void handleCompoundShapeSelectionChange(Observable*, int);

            std::unique_ptr<StatusBarController> statusBarController;

            QAction* saveAction;
            QAction* saveAsAction;
            QAction* closeAction;
            std::map<SceneDisplayer::ViewProperties, QAction*> viewActions;

            std::string mapEditorPath;
            std::string mapFilename;

            std::unique_ptr<SceneController> sceneController;
            SceneDisplayerWindow* sceneDisplayerWindow;
            ScenePanelWidget* scenePanelWidget;

        private slots:
            void showNewDialog();
            void showOpenDialog();
            void executeSaveAction();
            void showSaveAsDialog();
            bool executeCloseAction();
            void executeExitAction();

            void executeViewPropertiesChangeAction();
            static ScenePanelWidget::TabName getConcernedTabFor(SceneDisplayer::ViewProperties);
    };

}
