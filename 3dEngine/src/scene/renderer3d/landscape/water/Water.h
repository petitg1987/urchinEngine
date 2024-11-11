#pragma once

#include <UrchinCommon.h>
#include <memory>

#include <graphics/api/GraphicsApi.h>
#include <scene/renderer3d/camera/Camera.h>
#include <scene/renderer3d/landscape/fog/FogContainer.h>
#include <scene/renderer3d/landscape/fog/Fog.h>

namespace urchin {

    class Water final : public Observable {
        public:
            Water();
            ~Water() override = default;

            enum NotificationType {
                MOVE_UNDER_WATER,
                MOVE_ABOVE_WATER
            };

            void initialize(RenderTarget&);

            void setCenterPosition(const Point3<float>&);
            const Point3<float>& getCenterPosition() const;

            void setXSize(float);
            float getXSize() const;

            void setZSize(float);
            float getZSize() const;

            void setWaterColor(const Vector3<float>&);
            const Vector3<float>& getWaterColor() const;

            void setNormalTexture(std::string);
            const std::string& getNormalFilename() const;

            void setDudvMap(std::string);
            const std::string& getDudvFilename() const;

            void setWaveSpeed(float);
            float getWaveSpeed() const;

            void setWaveStrength(float);
            float getWaveStrength() const;

            void setSRepeat(float);
            float getSRepeat() const;

            void setTRepeat(float);
            float getTRepeat() const;

            void setDensity(float);
            float getDensity() const;

            void setGradient(float);
            float getGradient() const;

            void prepareRendering(unsigned int, const Camera&, FogContainer&, float);

        private:
            void updateRenderer();
            void buildUnderwaterFog();

            static constexpr uint32_t POSITIONING_DATA_UNIFORM_BINDING = 0;
            static constexpr uint32_t WATER_PROPS_UNIFORM_BINDING = 1;
            static constexpr uint32_t NORMAL_TEX_UNIFORM_BINDING = 2;
            static constexpr uint32_t DUDV_TEX_UNIFORM_BINDING = 3;

            bool isInitialized;

            RenderTarget* renderTarget;
            std::unique_ptr<Shader> waterShader;
            struct {
                alignas(16) Matrix4<float> projectionViewMatrix;
                alignas(4) float sumTimeStep = 0.0f;
            } positioningData;
            struct {
                alignas(16) Vector3<float> color;
                alignas(4) float waveSpeed;
                alignas(4) float waveStrength;
            } waterProperties;

            Point3<float> centerPosition;
            float xSize;
            float zSize;
            std::unique_ptr<Rectangle2D<float>> waterRectangle;

            std::string normalFilename;
            std::string dudvFilename;
            std::shared_ptr<Texture> normalTexture;
            std::shared_ptr<Texture> dudvMap;
            float sRepeat;
            float tRepeat;

            float density;
            float gradient;
            std::unique_ptr<Fog> underwaterFog;

            std::unique_ptr<GenericRenderer> waterRenderer;

    };

}
