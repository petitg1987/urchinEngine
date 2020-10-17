#ifndef URCHINENGINE_AMBIENTOCCLUSIONMANAGER_H
#define URCHINENGINE_AMBIENTOCCLUSIONMANAGER_H

#include <GL/glew.h>
#include "UrchinCommon.h"

#include "scene/renderer3d/camera/Camera.h"
#include "utils/display/quad/QuadDisplayer.h"
#include "utils/filter/bilateralblur/BilateralBlurFilter.h"

namespace urchin {

    class AmbientOcclusionManager {
        public:
            enum AOTextureSize
            {
                FULL_SIZE = 0,
                HALF_SIZE = 1
            };

            AmbientOcclusionManager(unsigned int, unsigned int);
            ~AmbientOcclusionManager();

            void loadUniformLocationFor(unsigned int);
            void onResize(unsigned int, unsigned int);
            void onCameraProjectionUpdate(const Camera *);

            void setTextureSize(AOTextureSize);
            void setKernelSamples(unsigned int);
            void setRadius(float);
            void setAmbientOcclusionStrength(float);
            void setDistanceAttenuation(float, float);
            void setNoiseTextureSize(unsigned int);
            void setBias(float);
            void setBlurSize(unsigned int);
            void setBlurSharpness(float);

            void activateBlur(bool);

            unsigned int getAmbientOcclusionTextureID() const;

            void updateAOTexture(const Camera *);
            void loadAOTexture(unsigned int) const;

        private:
            void createOrUpdateAOTexture();
            void createOrUpdateAOShader();
            void generateKernelSamples();
            void generateNoiseTexture();
            void exportSVG(const std::string &, const std::vector<Vector3<float>> &) const;

            int retrieveTextureSizeFactor();

            //scene information
            unsigned int sceneWidth, sceneHeight;
            float nearPlane, farPlane;
            float projectionScale;

            //tweak
            AOTextureSize textureSize;
            unsigned int textureSizeX, textureSizeY;
            unsigned int kernelSamples;
            float radius;
            float ambientOcclusionStrength;
            float depthStartAttenuation, depthEndAttenuation;
            unsigned int noiseTextureSize;
            float bias;
            unsigned int blurSize;
            float blurSharpness;

            //frame buffer object
            unsigned int fboID;
            unsigned int ambientOcclusionTexID;

            //ambient occlusion shader
            unsigned int ambientOcclusionShader;
            int mInverseViewProjectionLoc;
            int mProjectionLoc;
            int mViewLoc;
            int resolutionLoc;
            unsigned int noiseTexId;

            //visual data
            unsigned int depthTexID;
            unsigned int normalAndAmbientTexID;
            GLint ambientOcclusionTexLoc;
            std::shared_ptr<QuadDisplayer> quadDisplayer;

            std::shared_ptr<BilateralBlurFilter> verticalBlurFilter;
            std::shared_ptr<BilateralBlurFilter> horizontalBlurFilter;
            bool isBlurActivated;
    };

}

#endif
