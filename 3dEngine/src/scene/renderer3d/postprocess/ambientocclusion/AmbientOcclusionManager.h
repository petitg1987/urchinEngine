#ifndef URCHINENGINE_AMBIENTOCCLUSIONMANAGER_H
#define URCHINENGINE_AMBIENTOCCLUSIONMANAGER_H

#include "UrchinCommon.h"

#include "graphic/render/shader/model/Shader.h"
#include "scene/renderer3d/camera/Camera.h"
#include "graphic/render/GenericRenderer.h"
#include "texture/filter/bilateralblur/BilateralBlurFilter.h"

namespace urchin {

    class AmbientOcclusionManager {
        public:
            enum AOTextureSize {
                FULL_SIZE = 0,
                HALF_SIZE = 1
            };

            AmbientOcclusionManager();
            ~AmbientOcclusionManager();

            void onTexturesUpdate(const std::shared_ptr<Texture>&, const std::shared_ptr<Texture>&);
            void onResize(unsigned int, unsigned int);
            void onCameraProjectionUpdate(const Camera*);

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

            const std::shared_ptr<Texture>& getAmbientOcclusionTexture() const;

            void updateAOTexture(const Camera*);
            void loadAOTexture(const std::unique_ptr<GenericRenderer>&, std::size_t) const;

        private:
            void createOrUpdateAO();
            void createOrUpdateAOShader();
            void createOrUpdateAOTexture();
            void createOrUpdateRenderer();
            void generateKernelSamples();
            void generateNoiseTexture();
            void exportSVG(const std::string&, const std::vector<Vector4<float>>&) const;

            int retrieveTextureSizeFactor();

            //scene information
            float nearPlane, farPlane;

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
            std::shared_ptr<OffscreenRender> offscreenRenderTarget;
            std::shared_ptr<Texture> ambientOcclusionTexture;

            //ambient occlusion shader
            std::shared_ptr<Shader> ambientOcclusionShader;
            std::shared_ptr<Texture> noiseTexture;
            struct {
                alignas(16) Matrix4<float> inverseProjectionViewMatrix;
                alignas(16) Matrix4<float> projectionMatrix;
                alignas(16) Matrix4<float> viewMatrix;
            } positioningData;
            std::vector<Vector4<float>> ssaoKernel;
            Vector2<float> resolution;

            //visual data
            std::shared_ptr<Texture> depthTexture;
            std::shared_ptr<Texture> normalAndAmbientTexture;
            std::unique_ptr<GenericRenderer> renderer;

            std::unique_ptr<BilateralBlurFilter> verticalBlurFilter;
            std::unique_ptr<BilateralBlurFilter> horizontalBlurFilter;
            bool isBlurActivated;
    };

}

#endif
