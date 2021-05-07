#ifndef URCHINENGINE_SCENEDISPLAYERWINDOW_H
#define URCHINENGINE_SCENEDISPLAYERWINDOW_H

#include <string>
#include <QKeyEvent>
#include <QWindow>
#include <QVulkanInstance>

#include "UrchinCommon.h"
#include "SceneDisplayer.h"
#include "src/controller/SceneController.h"
#include "widget/controller/statusbar/StatusBarController.h"

namespace urchin {

    class SceneDisplayerWindow : public QWindow, public Observable {
        Q_OBJECT

        public:
            SceneDisplayerWindow(QWidget*, StatusBarController, std::string);
            ~SceneDisplayerWindow() override;

            enum NotificationType {
                BODY_PICKED
            };

            void exposeEvent(QExposeEvent *) override;
            bool event(QEvent *) override;

            void loadMap(SceneController*, const std::string&, const std::string&);
            void saveState(const std::string&) const;
            void closeMap();

            void setViewProperties(SceneDisplayer::ViewProperties, bool);
            void setHighlightSceneObject(const SceneObject*);
            void setHighlightCompoundShapeComponent(std::shared_ptr<const LocalizedCollisionShape>);
            void setHighlightSceneLight(const SceneLight*);
            void setHighlightSceneSound(const SceneSound*);

            void render();

            void keyPressEvent(QKeyEvent*) override;
            void keyReleaseEvent(QKeyEvent*) override;
            void mousePressEvent(QMouseEvent*) override;
            void mouseReleaseEvent(QMouseEvent*) override;
            void mouseMoveEvent(QMouseEvent*) override;

            bool onMouseClickBodyPickup();
            const std::string& getLastPickedBodyId() const;
            void addObserverObjectMoveController(Observer*, int);

        private:
            void updateSceneDisplayerViewProperties();

            QWidget* parent;
            StatusBarController statusBarController;
            std::string mapEditorPath;

            SceneDisplayer* sceneDisplayer;
            bool viewProperties[SceneDisplayer::LAST_VIEW_PROPERTIES];

            int mouseX, mouseY;
            std::string lastPickedBodyId;

        private slots:
            void onCtrlXPressed();
            void onCtrlYPressed();
            void onCtrlZPressed();
    };

}

#endif
