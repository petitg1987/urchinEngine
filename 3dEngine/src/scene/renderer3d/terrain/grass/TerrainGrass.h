#ifndef URCHINENGINE_TERRAINGRASS_H
#define URCHINENGINE_TERRAINGRASS_H

#include <string>
#include <vector>
#include "UrchinCommon.h"

#include "scene/renderer3d/terrain/grass/TerrainGrassQuadtree.h"
#include "scene/renderer3d/terrain/TerrainMesh.h"
#include "scene/renderer3d/camera/Camera.h"

namespace urchin
{

    class TerrainGrass
    {
        public:
            explicit TerrainGrass(const std::string &);
            ~TerrainGrass();

            void onCameraProjectionUpdate(const Matrix4<float> &);

            void initialize(const std::unique_ptr<TerrainMesh> &, const Point3<float> &);

            void display(const Camera *, float);

        private:
            void generateGrass(const std::unique_ptr<TerrainMesh> &, const Point3<float> &);
            unsigned int retrieveVertexIndex(const Point2<float> &, const std::unique_ptr<TerrainMesh> &mesh) const;
            void buildGrassQuadtree(const std::vector<TerrainGrassQuadtree *> &, unsigned int, unsigned int);

            bool isInitialized;

            const float grassPatchSize;
            const unsigned int grassQuadtreeDepth;

            unsigned int bufferIDs[2], vertexArrayObject;
            enum //buffer IDs indices
            {
                VAO_VERTEX_POSITION = 0,
                VAO_NORMAL
            };
            enum //shader input
            {
                SHADER_VERTEX_POSITION = 0,
                SHADER_NORMAL
            };
            unsigned int terrainGrassShader;
            int mProjectionLoc, mViewLoc, sumTimeStepLoc;

            Matrix4<float> projectionMatrix;
            float sumTimeStep;

            Image *grassTexture;
            TerrainGrassQuadtree *mainGrassQuadtree;
    };

}

#endif
