#pragma once

#include <UrchinCommon.h>
#include <memory>

#include <api/render/shader/model/Shader.h>
#include <api/render/target/RenderTarget.h>
#include <api/render/GenericRenderer.h>
#include <resources/image/Image.h>
#include <scene/renderer3d/camera/Camera.h>
#include <scene/renderer3d/landscape/fog/FogContainer.h>
#include <scene/renderer3d/landscape/fog/Fog.h>

namespace urchin {

    class Water : public Observable {
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

            void prepareRendering(unsigned int, const Camera&, FogContainer&, float invFrameRate);

        private:
            void updateRenderer();
            void buildUnderwaterFog();

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
            float xSize, zSize;
            std::unique_ptr<Rectangle2D<float>> waterRectangle;

            std::string normalFilename, dudvFilename;
            std::shared_ptr<Texture> normalTexture, dudvMap;
            float sRepeat, tRepeat;

            float density;
            float gradient;
            std::unique_ptr<Fog> underwaterFog;

            std::unique_ptr<GenericRenderer> waterRenderer;

    };

}
