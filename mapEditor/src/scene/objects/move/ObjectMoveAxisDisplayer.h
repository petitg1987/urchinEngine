#ifndef URCHINENGINE_OBJECTMOVEAXISDISPLAYER_H
#define URCHINENGINE_OBJECTMOVEAXISDISPLAYER_H

#include "Urchin3dEngine.h"
#include "UrchinMapHandler.h"

#include "controller/SceneController.h"
#include "widget/controller/statusbar/StatusBarController.h"

namespace urchin
{

    class ObjectMoveAxisDisplayer
    {
        public:
            ObjectMoveAxisDisplayer(SceneManager *, SceneController *, StatusBarController);
            ~ObjectMoveAxisDisplayer();

            void onCtrlXYZ(unsigned int);
            bool onMouseMove(int, int);
            bool onMouseLeftButton();
            bool onEscapeKey();

            void setSelectedSceneObject(const SceneObject *);

            void displayAxis();

        private:
            GeometryModel *createAxisModel(Model *, unsigned int);
            void cleanCurrentDisplay();

            bool isCameraMoved() const;
            void moveObject(const Point2<float> &, const Point2<float> &);
            void updateObjectPosition(const Point3<float> &);

            SceneManager *sceneManager;
            SceneController *sceneController;
            StatusBarController statusBarController;
            std::vector<GeometryModel *> objectMoveAxisModels;

            const SceneObject *selectedSceneObject;
            int selectedAxis;
            Point3<float> savedPosition;

            int oldMouseX, oldMouseY;
            Matrix4<float> oldCameraViewMatrix;
    };

}

#endif
