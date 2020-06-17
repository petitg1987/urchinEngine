#ifndef URCHINENGINE_OBJECTMOVECONTROLLER_H
#define URCHINENGINE_OBJECTMOVECONTROLLER_H

#include "UrchinCommon.h"
#include "Urchin3dEngine.h"
#include "UrchinMapHandler.h"

#include "controller/SceneController.h"
#include "scene/objects/move/ObjectMoveAxisDisplayer.h"
#include "widget/controller/statusbar/StatusBarController.h"
#include "widget/controller/mouse/MouseController.h"

namespace urchin
{

    class ObjectMoveController : public Observable
    {
        public:
            ObjectMoveController(SceneManager *, SceneController *, MouseController, StatusBarController);

            enum NotificationType
            {
                OBJECT_MOVED
            };

            void onResize(unsigned int, unsigned int);

            void onCtrlXYZ(unsigned int);
            bool onMouseMove(int, int);
            void onMouseOut();
            bool onMouseLeftButton();
            bool onEscapeKey();

            void setSelectedSceneObject(const SceneObject *);
            const SceneObject *getSelectedSceneObject() const;

            void displayAxis();

        private:
            bool isCameraMoved() const;
            bool adjustMousePosition();
            void moveObject(const Point2<float> &, const Point2<float> &);
            void updateObjectPosition(const Point3<float> &);

            unsigned int sceneWidth, sceneHeight;

            ObjectMoveAxisDisplayer objectMoveAxisDisplayer;
            SceneManager *sceneManager;
            SceneController *sceneController;
            MouseController mouseController;
            StatusBarController statusBarController;

            const SceneObject *selectedSceneObject;
            int selectedAxis;
            Point3<float> savedPosition;

            int oldMouseX, oldMouseY;
            Matrix4<float> oldCameraViewMatrix;
    };

}

#endif
