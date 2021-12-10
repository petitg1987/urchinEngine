#pragma once

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>
#include <UrchinMapHandler.h>

#include <controller/SceneController.h>
#include <scene/models/move/ModelMoveAxisDisplayer.h>
#include <widget/controller/statusbar/StatusBarController.h>
#include <widget/controller/mouse/MouseController.h>

namespace urchin {

    class ModelMoveController : public Observable { //TODO rename
        public:
            ModelMoveController(Scene&, SceneController&, MouseController&, StatusBarController&);

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
            void moveModel(const Point2<float>&, const Point2<float>&);
            void updateModelPosition(const Point3<float>&);

            unsigned int sceneWidth, sceneHeight;

            ModelMoveAxisDisplayer modelMoveAxisDisplayer;
            Scene& scene;
            SceneController& sceneController;
            MouseController& mouseController;
            StatusBarController& statusBarController;

            const ObjectEntity* selectedObjectEntity;
            int selectedAxis;
            Point3<float> savedPosition;

            double oldMouseX, oldMouseY;
            Matrix4<float> oldCameraViewMatrix;
    };

}
