#ifndef URCHINENGINE_AMBIENTOCCLUSIONMANAGER_H
#define URCHINENGINE_AMBIENTOCCLUSIONMANAGER_H

#include "UrchinCommon.h"

#include "graphic/shader/model/Shader.h"
#include "graphic/shader/model/ShaderVar.h"
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
            void refreshRenderer();
            void createOrUpdateAOTexture();
            void createOrUpdateAOShader();
            void generateKernelSamples();
            void generateNoiseTexture();
            void exportSVG(const std::string&, const std::vector<Vector3<float>>&) const;

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
            std::shared_ptr<OffscreenRender> offscreenRenderTarget;
            std::shared_ptr<Texture> ambientOcclusionTexture;

            //ambient occlusion shader
            std::unique_ptr<Shader> ambientOcclusionShader;
            ShaderVar mInverseViewProjectionShaderVar, mProjectionShaderVar, mViewShaderVar;
            std::shared_ptr<Texture> noiseTexture;

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
