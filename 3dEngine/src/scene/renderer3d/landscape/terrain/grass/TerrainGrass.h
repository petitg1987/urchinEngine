#ifndef URCHINENGINE_TERRAINGRASS_H
#define URCHINENGINE_TERRAINGRASS_H

#include <string>
#include <vector>
#include "UrchinCommon.h"

#include "graphic/shader/model/Shader.h"
#include "graphic/shader/model/ShaderVar.h"
#include "graphic/render/target/RenderTarget.h"
#include "scene/renderer3d/landscape/terrain/grass/TerrainGrassQuadtree.h"
#include "scene/renderer3d/landscape/terrain/TerrainMesh.h"
#include "scene/renderer3d/camera/Camera.h"

namespace urchin {

    class TerrainGrass {
        public:
            explicit TerrainGrass(const std::string&);
            ~TerrainGrass();

            void initialize(std::shared_ptr<RenderTarget>);
            bool isInitialized() const;
            void onCameraProjectionUpdate(const Matrix4<float>&);

            void refreshWith(const std::shared_ptr<TerrainMesh>&, const Point3<float>&);
            void refreshWith(float);

            const std::string& getGrassTexture() const;
            void setGrassTexture(const std::string&);

            const std::string& getMaskTexture() const;
            void setMaskTexture(const std::string&);

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
            void setWindDirection(const Vector3<float>&);

            float getWindStrength() const;
            void setWindStrength(float);

            void display(const Camera*, float);

        private:
            void generateGrass(const std::shared_ptr<TerrainMesh>&, const Point3<float>&);
            unsigned int retrieveVertexIndex(const Point2<float>&) const;
            void buildGrassQuadtree(const std::vector<TerrainGrassQuadtree*>&, unsigned int, unsigned int);
            void createRenderers(const std::vector<TerrainGrassQuadtree*>&);
            std::vector<GenericRenderer*> getAllRenderers() const;

            const float grassPositionRandomPercentage;
            const float grassPatchSize;
            const unsigned int grassQuadtreeDepth;

            bool bIsInitialized;
            std::shared_ptr<RenderTarget> renderTarget;

            std::shared_ptr<Shader> terrainGrassShader;
            ShaderVar mProjectionShaderVar, mViewShaderVar, cameraPositionShaderVar, sumTimeStepShaderVar;
            ShaderVar terrainMinPointShaderVar, terrainMaxPointShaderVar, terrainAmbientShaderVar;
            ShaderVar grassDisplayDistanceShaderVar;
            ShaderVar grassHeightShaderVar, grassLengthShaderVar, numGrassInTexShaderVar;
            ShaderVar windDirectionShaderVar, windStrengthShaderVar;
            struct {
                alignas(16) Matrix4<float> viewMatrix;
                alignas(16) Point3<float> cameraPosition;
                alignas(4) float sumTimeStep;
            } positioningData;
            struct {
                alignas(4) float displayDistance;
                alignas(4) float height;
                alignas(4) float length;
                alignas(4) int numGrassInTex;
                alignas(4) float windStrength;
                alignas(16) Vector3<float> windDirection;
            } grassProperties;
            struct {
                alignas(16) Point3<float> terrainMinPoint;
                alignas(16) Point3<float> terrainMaxPoint;
            } terrainPositioningData;
            Matrix4<float> projectionMatrix;
            float ambient;

            std::shared_ptr<TerrainMesh> mesh;
            Point3<float> terrainPosition;

            std::shared_ptr<Texture> grassTexture, grassMaskTexture;
            std::string grassTextureFilename, grassMaskFilename;
            TerrainGrassQuadtree* mainGrassQuadtree;
            mutable std::vector<const TerrainGrassQuadtree*> grassQuadtrees;
            float grassQuantity;
    };

}

#endif
