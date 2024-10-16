#pragma once

#include <string>
#include <map>

#include <texture/filter/TextureFilter.h>

namespace urchin {

    class GaussianBlur3dFilterBuilder;

    class GaussianBlur3dFilter final : public TextureFilter {
        public:
            enum BlurDirection {
                VERTICAL,
                HORIZONTAL
            };

            GaussianBlur3dFilter(const GaussianBlur3dFilterBuilder*, BlurDirection);

            void onCameraProjectionUpdate(float, float);

        private:
            struct GaussianBlur3dShaderConst {
                uint32_t kernelRadius;
                float blurSharpness;
            };

            std::string getShaderName() const override;
            void completeRenderer(const std::shared_ptr<GenericRendererBuilder>&, const std::shared_ptr<TextureReader>&) override;
            std::unique_ptr<ShaderConstants> buildShaderConstants() const override;

            void computeUvOffsets();

            static constexpr unsigned int KERNEL_RADIUS_SHADER_LIMIT = 9; //must be equals to 'NB_TEXTURE_FETCH' in texFilterGaussianBlur3d shader

            static constexpr uint32_t CAMERA_PLANES_UNIFORM_BINDING = 0;
            static constexpr uint32_t OFFSETS_DATA_UNIFORM_BINDING = 1;
            static constexpr uint32_t SRC_TEX_UNIFORM_BINDING = 2;
            static constexpr uint32_t DEPTH_TEX_UNIFORM_BINDING = 3;

            std::shared_ptr<Texture> depthTexture; //source texture

            BlurDirection blurDirection;
            unsigned int kernelRadius;
            float blurSharpness;
            std::vector<Vector2<float>> uvOffsets;

            unsigned int textureSize;
            struct {
                alignas(4) float nearPlane;
                alignas(4) float farPlane;
            } cameraPlanes;
    };

}
