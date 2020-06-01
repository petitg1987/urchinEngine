#ifndef URCHINENGINE_MAPEDITOR_TERRAINCONTROLLER_H
#define URCHINENGINE_MAPEDITOR_TERRAINCONTROLLER_H

#include <list>
#include "UrchinCommon.h"
#include "UrchinMapHandler.h"
#include "Urchin3dEngine.h"

#include "panel/AbstractController.h"

namespace urchin
{

    class TerrainController : public AbstractController
    {
        public:
            explicit TerrainController(MapHandler *);

            std::list<const SceneTerrain *> getSceneTerrains() const;
            void addSceneTerrain(SceneTerrain *);
            void removeSceneTerrain(const SceneTerrain *);

            const SceneTerrain *updateSceneTerrainGeneralProperties(const SceneTerrain *, const Point3<float> &, float);
            const SceneTerrain *updateSceneTerrainMesh(const SceneTerrain *, float, float);
            const SceneTerrain *updateSceneTerrainMaterial(const SceneTerrain *, float, float, const std::string&, const std::vector<std::string> &);
            const SceneTerrain *updateSceneTerrainGrass(const SceneTerrain *, const std::string&, const std::string&, unsigned int, float, float, float, const Vector3<float> &, float);

        private:
            SceneTerrain *findSceneTerrain(const SceneTerrain *);
    };

}

#endif
