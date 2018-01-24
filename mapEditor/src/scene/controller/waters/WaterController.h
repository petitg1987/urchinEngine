#ifndef URCHINENGINE_MAPEDITOR_WATERCONTROLLER_H
#define URCHINENGINE_MAPEDITOR_WATERCONTROLLER_H

#include <list>
#include "UrchinCommon.h"
#include "UrchinMapHandler.h"
#include "Urchin3dEngine.h"

namespace urchin
{

    class WaterController
    {
        public:
            explicit WaterController(MapHandler *);

            bool isModified() const;
            void resetModified();

            std::list<const SceneWater *> getSceneWaters() const;
            void addSceneWater(SceneWater *);
            void removeSceneWater(const SceneWater *);

            const SceneWater *updateSceneWater(const SceneWater *, const Point3<float> &, float, float, const Vector3<float> &, const std::string &, float, float);

        private:
            void markModified();
            SceneWater *findSceneWater(const SceneWater *);

            bool bIsModified;
            MapHandler *mapHandler;
    };

}

#endif
