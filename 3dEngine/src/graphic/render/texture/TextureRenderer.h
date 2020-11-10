#ifndef URCHINENGINE_TEXTURERENDERER_H
#define URCHINENGINE_TEXTURERENDERER_H

#include <memory>
#include "UrchinCommon.h"

#include "graphic/shader/model/Shader.h"
#include "graphic/shader/model/ShaderVar.h"
#include "graphic/render/GenericRenderer.h"

namespace urchin {

    /**
    * Display a frame buffer object
    * Sample of use:
    *     DisplayTexture displayTexture(textureIDs[TEX_DEPTH], DisplayTexture::DEPTH_FACTOR);
    *     displayTexture.setPosition(DisplayTexture::LEFT, DisplayTexture::BOTTOM);
    *    displayTexture.setFullscreen(false);
    *    displayTexture.display();
    */
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

            TextureRenderer(unsigned int, TextureRenderer::ColorType, float colorIntensity = 1.0);
            TextureRenderer(unsigned int, unsigned int, TextureRenderer::ColorType, float colorIntensity = 1.0);

            void setPosition(TextureRenderer::CoordinateX, TextureRenderer::CoordinateY);
            void setSize(float, float, float, float);
            void setFullScreen(bool);

            void initialize(unsigned int, unsigned int, float, float);

            void display();

        private:
            void initializeShader(float, float);

            bool isInitialized;

            TextureRenderer::CoordinateX coordinateX;
            TextureRenderer::CoordinateY coordinateY;
            bool fullScreen;
            float userMinX, userMaxX, userMinY, userMaxY;

            //visual
            unsigned int textureID;
            TextureRenderer::ColorType colorType;
            float colorIntensity;
            std::unique_ptr<GenericRenderer> renderer;

            Matrix3<float> mProjection;
            std::unique_ptr<Shader> displayTextureShader;
            int layer;
            ShaderVar mProjectionShaderVar, diffuseTexShaderVar;
    };

}

#endif

