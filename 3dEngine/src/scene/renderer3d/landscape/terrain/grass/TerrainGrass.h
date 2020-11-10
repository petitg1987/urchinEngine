#ifndef URCHINENGINE_TERRAINGRASS_H
#define URCHINENGINE_TERRAINGRASS_H

#include <string>
#include <vector>
#include "UrchinCommon.h"

#include "graphic/shader/model/Shader.h"
#include "graphic/shader/model/ShaderVar.h"
#include "scene/renderer3d/landscape/terrain/grass/TerrainGrassQuadtree.h"
#include "scene/renderer3d/landscape/terrain/TerrainMesh.h"
#include "scene/renderer3d/camera/Camera.h"

namespace urchin {

    class TerrainGrass {
        public:
            explicit TerrainGrass(const std::string &);
            ~TerrainGrass();

            void onCameraProjectionUpdate(const Matrix4<float> &);

            void refreshWith(const std::shared_ptr<TerrainMesh> &, const Point3<float> &);
            void refreshWith(float);

            const std::string &getGrassTexture() const;
            void setGrassTexture(const std::string &);

            const std::string &getMaskTexture() const;
            void setMaskTexture(const std::string &);

            float getGrassDisplayDistance() const;
            void setGrassDisplayDistance(float);

            float getGrassHeight() const;
            void setGrassHeight(float);

            float getGrassLength() const;
            void setGrassLength(float);

            unsigned int getNumGrassInTexture() const;
            void setNumGrassInTexture(unsigned int);

            float getGrassQuantity() const;
            void setGrassQuantity(float);

            Vector3<float> getWindDirection() const;
            void setWindDirection(const Vector3<float> &);

            float getWindStrength() const;
            void setWindStrength(float);

            void display(const Camera *, float);

        private:
            void generateGrass(const std::shared_ptr<TerrainMesh> &, const Point3<float> &);
            unsigned int retrieveVertexIndex(const Point2<float> &) const;
            void buildGrassQuadtree(const std::vector<TerrainGrassQuadtree *> &, unsigned int, unsigned int);
            void createVBO(const std::vector<TerrainGrassQuadtree *> &);
            void clearVBO();

            const float grassPositionRandomPercentage;
            const float grassPatchSize;
            const unsigned int grassQuadtreeDepth;

            std::vector<unsigned int> vertexArrayObjects;
            std::vector<std::array<unsigned int, 2>> bufferIDs;
            enum { //buffer IDs indices
                VAO_VERTEX_POSITION = 0,
                VAO_NORMAL
            };
            enum { //shader input
                SHADER_VERTEX_POSITION = 0,
                SHADER_NORMAL
            };
            std::unique_ptr<Shader> terrainGrassShader;
            ShaderVar mProjectionShaderVar, mViewShaderVar, cameraPositionShaderVar, sumTimeStepShaderVar;
            ShaderVar terrainMinPointShaderVar, terrainMaxPointShaderVar, terrainAmbientShaderVar;
            ShaderVar grassDisplayDistanceShaderVar;
            ShaderVar grassHeightShaderVar, grassHalfLengthShaderVar, numGrassInTexShaderVar;
            ShaderVar windDirectionShaderVar, windStrengthShaderVar;

            std::shared_ptr<TerrainMesh> mesh;
            Point3<float> terrainPosition;

            Matrix4<float> projectionMatrix;
            float sumTimeStep;

            Image *grassTexture, *grassMaskTexture;
            std::string grassTextureFilename, grassMaskFilename;
            TerrainGrassQuadtree *mainGrassQuadtree;
            std::vector<const TerrainGrassQuadtree *> grassQuadtrees;
            unsigned int numGrassInTex;
            float grassDisplayDistance;
            float grassHeight, grassLength;
            float grassQuantity;

            Vector3<float> windDirection;
            float windStrength;
    };

}

#endif
