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

            void refreshWith(const std::shared_ptr<TerrainMesh> &, const Point3<float> &, float);

            const std::string &getGrassTexture() const;
            void setGrassTexture(const std::string &);

            const std::string &getMaskTexture() const;
            void setMaskTexture(const std::string &);

            float getGrassHeight() const;
            void setGrassHeight(float);

            float getGrassLength() const;
            void setGrassLength(float);

            unsigned int getNumGrassInTexture() const;
            void setNumGrassInTexture(unsigned int);

            float getGrassOffset() const;
            void setGrassOffset(float);

            Vector3<float> getWindDirection() const;
            void setWindDirection(const Vector3<float> &);

            float getWindStrength() const;
            void setWindStrength(float);

            void display(const Camera *, float);

        private:
            void generateGrass(const std::shared_ptr<TerrainMesh> &, const Point3<float> &);
            unsigned int retrieveVertexIndex(const Point2<float> &) const;
            void buildGrassQuadtree(const std::vector<TerrainGrassQuadtree *> &, unsigned int, unsigned int);

            const float grassPositionRandomPercentage;
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
            int terrainMinPointLoc, terrainMaxPointLoc, terrainAmbientLoc;
            int grassHeightLoc, grassHalfLengthLoc, numGrassInTexLoc;
            int windDirectionLoc, windStrengthLoc;

            std::shared_ptr<TerrainMesh> mesh;
            Point3<float> terrainPosition;

            Matrix4<float> projectionMatrix;
            float sumTimeStep;

            Image *grassTexture, *grassMaskTexture;
            std::string grassTextureFilename, grassMaskFilename;
            TerrainGrassQuadtree *mainGrassQuadtree;
            float grassHeight, grassLength;
            unsigned int numGrassInTex;
            float grassOffset;

            Vector3<float> windDirection;
            float windStrength;
    };

}

#endif
