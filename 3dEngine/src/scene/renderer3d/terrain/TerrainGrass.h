#ifndef URCHINENGINE_TERRAINGRASS_H
#define URCHINENGINE_TERRAINGRASS_H

#include <string>
#include "UrchinCommon.h"

#include "scene/renderer3d/terrain/TerrainMesh.h"

namespace urchin
{

    class TerrainGrass
    {
        public:
            explicit TerrainGrass(const std::string &);
            ~TerrainGrass();

            void onCameraProjectionUpdate(const Matrix4<float> &);

            void initialize(const std::unique_ptr<TerrainMesh> &, const Point3<float> &);

            void display(const Matrix4<float> &, float);

        private:
            std::vector<Point3<float>> generateGrassVertices(const std::unique_ptr<TerrainMesh> &, const Point3<float> &) const;
            Point3<float> retrieveGlobalVertex(const Point2<float> &, const std::unique_ptr<TerrainMesh> &mesh, const Point3<float> &) const;

            bool isInitialized;

            unsigned int bufferIDs[5], vertexArrayObject;
            enum //buffer IDs indices
            {
                VAO_VERTEX_POSITION = 0
            };
            enum //shader input
            {
                SHADER_VERTEX_POSITION = 0
            };
            unsigned int terrainGrassShader;
            int mProjectionLoc, mViewLoc, sumTimeStepLoc;

            Matrix4<float> projectionMatrix;
            float sumTimeStep;

            Image *grassTexture;
            unsigned int grassQuantity;
    };

}

#endif
