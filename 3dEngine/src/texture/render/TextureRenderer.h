#ifndef URCHINENGINE_TEXTURERENDERER_H
#define URCHINENGINE_TEXTURERENDERER_H

#include <memory>
#include "UrchinCommon.h"

#include "graphic/shader/model/Shader.h"
#include "graphic/shader/model/ShaderVar.h"
#include "graphic/render/GenericRenderer.h"
#include "graphic/render/target/RenderTarget.h"

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
                DEPTH_VALUE,
                GRAYSCALE_VALUE,
                INVERSE_GRAYSCALE_VALUE
            };

            TextureRenderer(std::shared_ptr<Texture>, TextureRenderer::ColorType, float colorIntensity = 1.0);
            TextureRenderer(std::shared_ptr<Texture>, unsigned int, TextureRenderer::ColorType, float colorIntensity = 1.0);

            void setPosition(TextureRenderer::CoordinateX, TextureRenderer::CoordinateY);
            void setSize(float, float, float, float);
            void setFullScreen(bool);
            void enableTransparency();

            void initialize(const std::shared_ptr<RenderTarget> &renderTarget, unsigned int, unsigned int, float, float);

            void display();

        private:
            void initializeShader(float, float);

            bool isInitialized;

            TextureRenderer::CoordinateX coordinateX;
            TextureRenderer::CoordinateY coordinateY;
            bool fullScreen;
            bool transparencyEnabled;
            float userMinX, userMaxX, userMinY, userMaxY;

            //visual
            std::shared_ptr<RenderTarget> renderTarget;
            std::shared_ptr<Texture> texture;
            TextureRenderer::ColorType colorType;
            float colorIntensity;
            std::unique_ptr<GenericRenderer> renderer;

            Matrix3<float> mProjection;
            std::shared_ptr<Shader> displayTextureShader;
            int layer;
            ShaderVar mProjectionShaderVar, diffuseTexShaderVar;
    };

}

#endif

