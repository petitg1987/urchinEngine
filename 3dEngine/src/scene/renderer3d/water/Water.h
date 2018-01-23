#ifndef URCHINENGINE_WATER_H
#define URCHINENGINE_WATER_H

#include "UrchinCommon.h"

#include "resources/image/Image.h"
#include "scene/renderer3d/camera/Camera.h"

namespace urchin
{

    class Water
    {
        public:
            Water(const Point3<float> &, float, float);
            ~Water();

            const Point3<float> &getCenterPosition() const;
            float getXSize() const;
            float getZSize() const;

            void setWaterColor(const Vector3<float> &);
            const Vector3<float> getWaterColor() const;

            void setNormalTexture(const std::string &);
            const Image *getNormalTexture() const;

            void setSRepeat(float);
            float getSRepeat() const;

            void setTRepeat(float);
            float getTRepeat() const;

            void onCameraProjectionUpdate(const Matrix4<float> &);

            void display(const Camera *, float invFrameRate);

        private:
            void generateVertex();

            unsigned int bufferIDs[3], vertexArrayObject;
            enum //buffer IDs indices
            {
                VAO_VERTEX_POSITION = 0,
                VAO_TEX_COORD
            };
            enum //shader input
            {
                SHADER_VERTEX_POSITION = 0,
                SHADER_TEX_COORD
            };
            unsigned int waterShader;
            int mProjectionLoc, mViewLoc, sumTimeStepLoc;
            int waterColorLoc;

            Matrix4<float> projectionMatrix;
            float sumTimeStep;

            Point3<float> centerPosition;
            float xSize, zSize;
            Vector3<float> waterColor;
            Image *normalTexture;
            float sRepeat, tRepeat;
    };

}

#endif
