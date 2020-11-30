#ifndef URCHINENGINE_TERRAINMANAGER_H
#define URCHINENGINE_TERRAINMANAGER_H

#include "UrchinCommon.h"

#include "graphic/render/target/RenderTarget.h"
#include "scene/renderer3d/camera/Camera.h"
#include "scene/renderer3d/landscape/terrain/Terrain.h"

namespace urchin {

    class TerrainManager {
        public:
            explicit TerrainManager(const RenderTarget*);

            void onCameraProjectionUpdate(const Camera*);

            void addTerrain(Terrain*);
            void removeTerrain(Terrain*);

            void setGrassDisplayDistance(float);

            void display(const Camera*, float) const;

        private:
            void updateWithConfig();

            const RenderTarget* renderTarget;

            std::vector<Terrain*> terrains;

            Matrix4<float> projectionMatrix;

            float grassDisplayDistance;
    };

}

#endif
