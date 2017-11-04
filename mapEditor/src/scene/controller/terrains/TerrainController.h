#ifndef URCHINENGINE_MAPEDITOR_TERRAINCONTROLLER_H
#define URCHINENGINE_MAPEDITOR_TERRAINCONTROLLER_H

#include <list>
#include "UrchinCommon.h"
#include "UrchinMapHandler.h"
#include "Urchin3dEngine.h"

namespace urchin
{

    class TerrainController
    {
        public:
            explicit TerrainController(MapHandler *);

            bool isModified() const;
            void resetModified();

            std::list<const SceneTerrain *> getSceneTerrains() const;
            void addSceneTerrain(SceneTerrain *);
            void removeSceneTerrain(const SceneTerrain *);

        private:
            void markModified();
            SceneTerrain *findSceneTerrain(const SceneTerrain *);

            bool bIsModified;
            MapHandler *mapHandler;
    };

}

#endif
