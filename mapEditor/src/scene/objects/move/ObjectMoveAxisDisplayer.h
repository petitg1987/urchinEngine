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
            ObjectMoveAxisDisplayer(SceneManager *);
            ~ObjectMoveAxisDisplayer();

            void displayAxis(const Point3<float> &, unsigned int);
            void cleanCurrentDisplay();

        private:
            GeometryModel *createAxisModel(const Point3<float> &, unsigned int, unsigned int);

            SceneManager *sceneManager;
            std::vector<GeometryModel *> objectMoveAxisModels;
    };

}

#endif
