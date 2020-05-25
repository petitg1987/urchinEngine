#ifndef URCHINENGINE_OBJECTMOVEAXISDISPLAYER_H
#define URCHINENGINE_OBJECTMOVEAXISDISPLAYER_H

#include "Urchin3dEngine.h"
#include "UrchinMapHandler.h"

namespace urchin
{

    class ObjectMoveAxisDisplayer
    {
        public:
            explicit ObjectMoveAxisDisplayer(SceneManager *);
            ~ObjectMoveAxisDisplayer();

            void startMove(unsigned int);
            bool onMouseMove(int, int);
            bool onMouseLeftButton();
            bool onEscapeKey();

            void displayAxisFor(const SceneObject *);

        private:
            GeometryModel *createAxisModel(Model *, unsigned int);
            void cleanCurrentDisplay();

            bool isCameraMoved() const;
            void moveObject(const Point2<float> &, const Point2<float> &);
            void updateObjectPosition(const Point3<float> &);

            SceneManager *sceneManager;
            std::vector<GeometryModel *> objectMoveAxisModels;

            const SceneObject *displayedObject;
            int selectedAxis;

            int oldMouseX, oldMouseY;
            Matrix4<float> oldCameraViewMatrix;
    };

}

#endif
