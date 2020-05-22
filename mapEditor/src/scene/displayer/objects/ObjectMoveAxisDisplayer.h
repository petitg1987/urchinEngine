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

            void displayAxisFor(const SceneObject *);

        private:
            GeometryModel *createAxisModel(Model *, unsigned int);
            void cleanCurrentDisplay();

            SceneManager *sceneManager;
            std::vector<GeometryModel *> objectMoveAxisModels;
    };

}

#endif
