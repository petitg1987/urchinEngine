#ifndef GREENCITY_TERRAINDISPLAYER_H
#define GREENCITY_TERRAINDISPLAYER_H

#include "UrchinCommon.h"

#include "scene/renderer3d/camera/Camera.h"
#include "scene/renderer3d/terrain/Terrain.h"

namespace urchin
{

    class TerrainDisplayer
    {
        public:
            void addTerrain(Terrain *);
            void removeTerrain(Terrain *);

            void onCameraProjectionUpdate(const Camera *);

            void display(const Matrix4<float> &, float) const;

        private:
            std::vector<Terrain *> terrains;

            Matrix4<float> projectionMatrix;
    };

}

#endif
