#pragma once

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>
#include <UrchinAggregation.h>

#include <controller/SceneController.h>
#include <scene/objects/move/ObjectMoveAxisDisplayer.h>
#include <widget/controller/statusbar/StatusBarController.h>
#include <widget/controller/mouse/MouseController.h>

namespace urchin {

    class ObjectMoveController final : public Observable {
        public:
            ObjectMoveController(Scene&, SceneController&, MouseController&, StatusBarController&);

            enum NotificationType {
                OBJECT_MOVED
            };

            void onResize(unsigned int, unsigned int);

            void onCtrlXYZ(unsigned int);
            bool onMouseMove(double, double);
            void onMouseOut();
            bool onMouseLeftButton();
            bool onEscapeKey();

            void setSelectedObjectEntity(const ObjectEntity*);
            const ObjectEntity* getSelectedObjectEntity() const;

            void displayAxis();

        private:
            bool isCameraMoved() const;
            bool adjustMousePosition();
            void moveObject(const Point2<float>&, const Point2<float>&);
            void updateObjectPosition(const Point3<float>&);

            unsigned int sceneWidth;
            unsigned int sceneHeight;

            ObjectMoveAxisDisplayer objectMoveAxisDisplayer;
            Scene& scene;
            SceneController& sceneController;
            MouseController& mouseController;
            StatusBarController& statusBarController;

            const ObjectEntity* selectedObjectEntity;
            int selectedAxis;
            Point3<float> savedPosition;

            double oldMouseX;
            double oldMouseY;
            Matrix4<float> oldCameraViewMatrix;
    };

}
