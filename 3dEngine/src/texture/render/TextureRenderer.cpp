#include <stdexcept>
#include <utility>

#include "TextureRenderer.h"
#include "graphic/shader/builder/ShaderBuilder.h"
#include "graphic/shader/data/ShaderDataSender.h"
#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    TextureRenderer::TextureRenderer(std::shared_ptr<Texture> texture, TextureRenderer::ColorType colorType, float colorIntensity) :
            isInitialized(false),
            coordinateX(TextureRenderer::LEFT),
            coordinateY(TextureRenderer::BOTTOM),
            fullScreen(false),
            transparencyEnabled(false),
            userMinX(0.0),
            userMaxX(1.0),
            userMinY(0.0),
            userMaxY(1.0),
            texture(std::move(texture)),
            colorType(colorType),
            colorIntensity(colorIntensity),
            layer(-1) {

    }

    TextureRenderer::TextureRenderer(std::shared_ptr<Texture> texture, unsigned int layer, TextureRenderer::ColorType colorType, float colorIntensity) :
            isInitialized(false),
            coordinateX(TextureRenderer::LEFT),
            coordinateY(TextureRenderer::BOTTOM),
            fullScreen(false),
            transparencyEnabled(false),
            userMinX(0.0),
            userMaxX(1.0),
            userMinY(0.0),
            userMaxY(1.0),
            texture(std::move(texture)),
            colorType(colorType),
            colorIntensity(colorIntensity),
            layer((int)layer) {
        assert(this->texture->getTextureType() == TextureType::ARRAY);
    }

    void TextureRenderer::setPosition(TextureRenderer::CoordinateX coordinateX, TextureRenderer::CoordinateY coordinateY) {
        if (isInitialized) {
            throw std::runtime_error("No position update allowed after initialization.");
        }

        this->coordinateX = coordinateX;
        this->coordinateY = coordinateY;
    }

    /**
     * @param minX Minimum X (0 : left screen)
     * @param maxX Maximum X (scene width : right screen)
     * @param minY Minimum Y (0 : top screen
     * @param maxY Maximum Y (scene height : bottom screen)
     */
    void TextureRenderer::setSize(float minX, float maxX, float minY, float maxY) {
        if (isInitialized) {
            throw std::runtime_error("No size update allowed after initialization.");
        }

        userMinX = minX;
        userMaxX = maxX;
        userMinY = minY;
        userMaxY = maxY;
    }

    void TextureRenderer::setFullScreen(bool fullScreen) {
        if (isInitialized) {
            throw std::runtime_error("No full screen flag update allowed after initialization.");
        }

        this->fullScreen = fullScreen;
    }

    void TextureRenderer::enableTransparency() {
        if (isInitialized) {
            throw std::runtime_error("No transparency flag update allowed after initialization.");
        }

        this->transparencyEnabled = true;
    }

    void TextureRenderer::initialize(unsigned int sceneWidth, unsigned int sceneHeight, float nearPlane, float farPlane) {
        if (isInitialized) {
            throw std::runtime_error("Texture displayer cannot be initialized twice.");
        }

        initializeShader(nearPlane, farPlane);

        float minX, maxX, minY, maxY;
        if (fullScreen) {
            minX = 0.0f;
            maxX = (float)sceneWidth;
            minY = 0.0f;
            maxY = (float)sceneHeight;
        } else {
            switch(coordinateX) {
                case TextureRenderer::LEFT:
                    minX = (float)sceneWidth * 0.025f;
                    maxX = (float)sceneWidth * 0.325f;
                    break;
                case TextureRenderer::CENTER_X:
                    minX = (float)sceneWidth * 0.35f;
                    maxX = (float)sceneWidth * 0.65f;
                    break;
                case TextureRenderer::RIGHT:
                    minX = (float)sceneWidth * 0.675f;
                    maxX = (float)sceneWidth * 0.975f;
                    break;
                case TextureRenderer::USER_DEFINED_X:
                    minX = userMinX;
                    maxX = userMaxX;
                    break;
                default:
                    throw std::domain_error("Unsupported coordinate X");
            }

            switch(coordinateY) {
                case TextureRenderer::BOTTOM:
                    minY = (float)sceneHeight * 0.675f;
                    maxY = (float)sceneHeight * 0.975f;
                    break;
                case TextureRenderer::CENTER_Y:
                    minY = (float)sceneHeight * 0.35f;
                    maxY = (float)sceneHeight * 0.65f;
                    break;
                case TextureRenderer::TOP:
                    minY = (float)sceneHeight * 0.025f;
                    maxY = (float)sceneHeight * 0.325f;
                    break;
                case TextureRenderer::USER_DEFINED_Y:
                    minY = userMinY;
                    maxY = userMaxY;
                    break;
                default:
                    throw std::domain_error("Unsupported coordinate Y");
            }
        }

        //orthogonal matrix with origin at top left screen
        mProjection.setValues(2.0f/(float)sceneWidth, 0.0f, -1.0f,
                0.0f, -2.0f/(float)sceneHeight, 1.0f,
                0.0f, 0.0f, 1.0f);
        ShaderDataSender().sendData(mProjectionShaderVar, mProjection);

        //update the display
        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(minX, minY), Point2<float>(maxX, minY), Point2<float>(maxX, maxY),
                Point2<float>(minX, minY), Point2<float>(maxX, maxY), Point2<float>(minX, maxY)
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.0f, 0.0f),
                Point2<float>(0.0f, 1.0f), Point2<float>(1.0f, 0.0f), Point2<float>(0.0f, 0.0f)
        };
        std::unique_ptr<GenericRendererBuilder> rendererBuilder = std::make_unique<GenericRendererBuilder>(ShapeType::TRIANGLE);
        rendererBuilder
                ->addData(&vertexCoord)
                ->addData(&textureCoord)
                ->addTexture(TextureReader::build(texture, TextureParam::buildNearest()));
        if(transparencyEnabled) {
            rendererBuilder->enableTransparency();
        }
        renderer = rendererBuilder->build();

        isInitialized = true;
    }

    void TextureRenderer::initializeShader(float nearPlane, float farPlane) {
        std::map<std::string, std::string> textureDisplayTokens;
        textureDisplayTokens["IS_DEFAULT_VALUE"] = colorType == ColorType::DEFAULT_VALUE ? "true" : "false";
        textureDisplayTokens["IS_DEPTH_VALUE"] = colorType == ColorType::DEPTH_VALUE ? "true" : "false";
        textureDisplayTokens["IS_GRAYSCALE_VALUE"] = colorType == ColorType::GRAYSCALE_VALUE ? "true" : "false";
        textureDisplayTokens["IS_INVERSE_GRAYSCALE_VALUE"] = colorType == ColorType::INVERSE_GRAYSCALE_VALUE ? "true" : "false";

        const std::string& fragShaderName = (layer == -1) ? "displayTexture.frag" : "displayTextureArray.frag";
        displayTextureShader = ShaderBuilder().createShader("displayTexture.vert", "", fragShaderName, textureDisplayTokens);

        float cameraPlanes[2] = {nearPlane, farPlane};
        int colorTexUnit = 0;
        ShaderDataSender()
            .sendData(ShaderVar(displayTextureShader, "colorIntensity"), colorIntensity)
            .sendData(ShaderVar(displayTextureShader, "cameraPlanes"), 2, cameraPlanes)
            .sendData(ShaderVar(displayTextureShader, "colorTex"), colorTexUnit);

        if (layer != -1) {
            ShaderDataSender().sendData(ShaderVar(displayTextureShader, "layer"), layer);
        }

        mProjectionShaderVar = ShaderVar(displayTextureShader, "mProjection");
    }

    void TextureRenderer::display(const RenderTarget* renderTarget) {
        if (!isInitialized) {
            throw std::runtime_error("Texture displayer must be initialized before display.");
        }

        renderTarget->activeShader(displayTextureShader);
        renderTarget->display(renderer);
    }

}
