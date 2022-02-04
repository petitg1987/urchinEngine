#pragma once

#include <memory>
#include <UrchinCommon.h>

#include <api/render/shader/model/Shader.h>
#include <api/render/GenericRenderer.h>
#include <api/render/target/RenderTarget.h>

namespace urchin {

    class TextureRenderer {
        public:
            enum CoordinateX {
                LEFT,
                CENTER_X,
                RIGHT,
                USER_DEFINED_X
            };

            enum CoordinateY {
                TOP,
                CENTER_Y,
                BOTTOM,
                USER_DEFINED_Y
            };

            enum ColorType {
                DEFAULT_VALUE,
                GRAYSCALE_VALUE,
                INVERSE_GRAYSCALE_VALUE
            };

            TextureRenderer(std::shared_ptr<Texture>, TextureRenderer::ColorType);
            TextureRenderer(std::shared_ptr<Texture>, unsigned int, TextureRenderer::ColorType);

            void setPosition(TextureRenderer::CoordinateX, TextureRenderer::CoordinateY);
            void setSize(float, float, float, float);
            void setFullScreen(bool);
            void enableTransparency();

            void initialize(std::string, RenderTarget& renderTarget, unsigned int, unsigned int, float = 0.0f, float = 1.0f);

            void prepareRendering(unsigned int);

        private:
            struct TextureRendererShaderConst {
                uint32_t isDefaultValue;
                uint32_t isGrayscaleValue;
                uint32_t isInverseGrayscaleValue;
            };

            void initializeShader();

            bool isInitialized;

            TextureRenderer::CoordinateX coordinateX;
            TextureRenderer::CoordinateY coordinateY;
            bool fullScreen;
            bool transparencyEnabled;
            float userMinX, userMaxX, userMinY, userMaxY;

            //visual
            std::shared_ptr<Texture> texture;
            TextureRenderer::ColorType colorType;
            std::unique_ptr<GenericRenderer> renderer;

            std::unique_ptr<Shader> displayTextureShader;
            struct {
                alignas(4) float minColorRange;
                alignas(4) float maxColorRange;
                alignas(4) int layer;
            } renderingData;
            Matrix4<float> mProjection;
    };

}

