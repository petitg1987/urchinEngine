#pragma once

#include <string>
#include <QKeyEvent>
#include <QWindow>
#include <UrchinCommon.h>

#include <scene/SceneDisplayer.h>
#include <scene/SceneWindowController.h>
#include <controller/SceneController.h>
#include <widget/controller/statusbar/StatusBarController.h>

namespace urchin {

    class SceneDisplayerWindow final : public QWindow, public Observable {
        Q_OBJECT

        public:
            SceneDisplayerWindow(QWidget*, StatusBarController&, std::string);
            ~SceneDisplayerWindow() override = default;

            enum NotificationType {
                BODY_PICKED
            };

            void exposeEvent(QExposeEvent *) override;
            bool event(QEvent *) override;
            void setupVkInstance(VkInstance);

            void loadMap(SceneController&, const std::string&, const std::string&);
            void loadEmptyScene();
            void saveState(const std::string&) const;
            void closeMap();

            void setViewProperties(SceneDisplayer::ViewProperties, bool);
            void setHighlightObjectPhysicsShapes(std::vector<const ObjectEntity*>) const;
            void setHighlightCompoundShapeComponent(const LocalizedCollisionShape*) const;
            void setHighlightObjectLights(std::vector<const ObjectEntity*>) const;
            void setHighlightObjectSounds(std::vector<const ObjectEntity*>) const;

            void render();

            void keyPressEvent(QKeyEvent*) override;
            void keyReleaseEvent(QKeyEvent*) override;
            void mousePressEvent(QMouseEvent*) override;
            void mouseReleaseEvent(QMouseEvent*) override;
            void mouseMoveEvent(QMouseEvent*) override;

            bool onMouseClickBodyPickup();
            const std::string& getLastPickedBodyId() const;
            void addObserverObjectMoveController(Observer*, int) const;

        private:
            void clearVkInstance();
            void updateSceneDisplayerViewProperties() const;

            QVulkanInstance vulkanInstance;
            StatusBarController& statusBarController;
            std::string mapEditorPath;

            SceneWindowController sceneWindowController;
            MouseController mouseController;
            std::unique_ptr<SceneDisplayer> sceneDisplayer;
            std::array<bool, SceneDisplayer::LAST_VIEW_PROPERTIES> viewProperties;

            bool forwardKeyPressed;
            bool backwardKeyPressed;
            bool leftKeyPressed;
            bool rightKeyPressed;
            double mouseX;
            double mouseY;
            std::string lastPickedBodyId;

        private slots:
            void onCtrlXPressed() const;
            void onCtrlYPressed() const;
            void onCtrlZPressed() const;
    };

}
