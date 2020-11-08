#ifndef URCHINENGINE_WATER_H
#define URCHINENGINE_WATER_H

#include "UrchinCommon.h"
#include <memory>

#include "graphic/render/GenericRenderer.h"
#include "resources/image/Image.h"
#include "scene/renderer3d/camera/Camera.h"
#include "scene/renderer3d/landscape/fog/FogManager.h"
#include "scene/renderer3d/landscape/fog/Fog.h"

namespace urchin {

    class Water : public Observable {
        public:
            Water();
            ~Water() override;

            enum NotificationType
            {
                MOVE_UNDER_WATER,
                MOVE_ABOVE_WATER,
            };

            void setCenterPosition(const Point3<float> &);
            const Point3<float> &getCenterPosition() const;

            void setXSize(float);
            float getXSize() const;

            void setZSize(float);
            float getZSize() const;

            void setWaterColor(const Vector3<float> &);
            const Vector3<float> &getWaterColor() const;

            void setNormalTexture(const std::string &);
            const Image *getNormalTexture() const;

            void setDudvMap(const std::string &);
            const Image *getDudvMap() const;

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

            void onCameraProjectionUpdate(const Matrix4<float> &);

            void display(const Camera *, FogManager *, float invFrameRate);

        private:
            void generateVertex();
            void updateWaterTextures();
            void buildUnderwaterFog();

            unsigned int waterShader;
            int mProjectionLoc, mViewLoc, sumTimeStepLoc;
            int waterColorLoc, waveSpeedLoc, waveStrengthLoc;

            Matrix4<float> projectionMatrix;
            float sumTimeStep;

            Point3<float> centerPosition;
            float xSize, zSize;
            std::unique_ptr<Rectangle<float>> waterRectangle;

            Vector3<float> waterColor;
            Image *normalTexture, *dudvMap;
            float waveSpeed, waveStrength;
            float sRepeat, tRepeat;

            float density;
            float gradient;
            std::shared_ptr<Fog> underwaterFog; //TODO unique ?

            std::unique_ptr<GenericRenderer> waterRenderer;

    };

}

#endif
