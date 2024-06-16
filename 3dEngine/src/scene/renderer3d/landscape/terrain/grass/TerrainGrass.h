#pragma once

#include <string>
#include <vector>
#include <UrchinCommon.h>

#include <graphics/api/GraphicsApi.h>
#include <scene/renderer3d/landscape/terrain/grass/TerrainGrassQuadtree.h>
#include <scene/renderer3d/landscape/terrain/TerrainMesh.h>
#include <scene/renderer3d/camera/Camera.h>

namespace urchin {

    class TerrainGrass {
        public:
            explicit TerrainGrass(const std::string&);

            void initialize(RenderTarget&);
            bool isInitialized() const;

            void refreshWith(const TerrainMesh*, const Point3<float>&);
            void refreshWith(float);

            const std::string& getGrassTexture() const;
            void setGrassTexture(std::string);

            const std::string& getMaskTexture() const;
            void setMaskTexture(std::string);

            float getGrassDisplayDistance() const;
            void setGrassDisplayDistance(float);

            float getGrassHeight() const;
            void setGrassHeight(float);

            float getGrassWidth() const;
            void setGrassWidth(float);

            unsigned int getNumGrassInTexture() const;
            void setNumGrassInTexture(unsigned int);

            float getGrassQuantity() const;
            void setGrassQuantity(float);

            Vector3<float> getWindDirection() const;
            void setWindDirection(const Vector3<float>&);

            float getWindStrength() const;
            void setWindStrength(float);

            void prepareRendering(unsigned int, const Camera&, float);

        private:
            void generateGrass(const TerrainMesh*, const Point3<float>&);
            bool discardGrass(const Image*, float, float, float, float) const;
            unsigned int retrieveVertexIndex(const Point2<float>&) const;
            void buildGrassQuadtree(std::vector<std::unique_ptr<TerrainGrassQuadtree>>, unsigned int, unsigned int);
            void createRenderers(const std::vector<std::unique_ptr<TerrainGrassQuadtree>>&);
            std::vector<GenericRenderer*> getAllRenderers() const;

            //Grass are positioned randomly in function of a percentage:
            // - 0%: no random in positioning
            // - 50%: two grass can have the same position (-50% on one grass and +50% on another grass)
            static constexpr float GRASS_POSITION_RANDOM_PERCENTAGE = 0.35f;

            static constexpr uint32_t POSITIONING_DATA_UNIFORM_BINDING = 0;
            static constexpr uint32_t GRASS_PROPS_UNIFORM_BINDING = 1;
            static constexpr uint32_t AMBIENT_UNIFORM_BINDING = 2;
            static constexpr uint32_t GRASS_TEX_UNIFORM_BINDING = 3;

            const float grassParcelSize;
            const unsigned int grassQuadtreeDepth;

            bool bIsInitialized;
            RenderTarget* renderTarget;

            std::unique_ptr<Shader> terrainGrassShader;
            struct {
                alignas(16) Matrix4<float> projectionViewMatrix;
                alignas(16) Point3<float> cameraPosition;
                alignas(4) float sumTimeStep;
            } positioningData;

            float ambient;
            float grassWidth;
            int numGrassInTex;
            float grassQuantity;
            struct {
                alignas(4) float displayDistance;
                alignas(4) float grassHeight;
                alignas(4) float windStrength;
                alignas(16) Vector3<float> windDirection;
            } grassProperties;

            Matrix4<float> projectionMatrix;
            const TerrainMesh* mesh;
            Point3<float> terrainPosition;

            std::shared_ptr<Texture> grassTexture;
            TextureParam grassTextureParam;
            std::string grassTextureFilename;
            std::string grassMaskFilename;
            std::unique_ptr<TerrainGrassQuadtree> mainGrassQuadtree;
            mutable std::vector<const TerrainGrassQuadtree*> grassQuadtrees;
    };

}
