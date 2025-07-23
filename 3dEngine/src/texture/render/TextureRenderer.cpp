#include <stdexcept>
#include <utility>
#include <cstring>

#include "texture/render/TextureRenderer.h"
#include "graphics/render/shader/ShaderBuilder.h"
#include "graphics/render/GenericRendererBuilder.h"

namespace urchin {

    TextureRenderer::TextureRenderer(std::shared_ptr<Texture> texture, ColorType colorType) :
            isInitialized(false),
            coordinateX(LEFT),
            coordinateY(BOTTOM),
            fullScreen(false),
            transparencyEnabled(false),
            userMinX(0.0),
            userMaxX(1.0),
            userMinY(0.0),
            userMaxY(1.0),
            texture(std::move(texture)),
            colorType(colorType),
            renderingData({}) {
        std::memset(&renderingData, 0, sizeof(renderingData));

        renderingData.minColorRange = 0.0f;
        renderingData.maxColorRange = 1.0f;
        renderingData.layer = -1;
    }

    TextureRenderer::TextureRenderer(std::shared_ptr<Texture> texture, unsigned int layer, ColorType colorType) :
            isInitialized(false),
            coordinateX(LEFT),
            coordinateY(BOTTOM),
            fullScreen(false),
            transparencyEnabled(false),
            userMinX(0.0),
            userMaxX(1.0),
            userMinY(0.0),
            userMaxY(1.0),
            texture(std::move(texture)),
            colorType(colorType),
            renderingData({}) {
        assert(this->texture->getTextureType() == TextureType::ARRAY);

        renderingData.minColorRange = 0.0f;
        renderingData.maxColorRange = 1.0f;
        renderingData.layer = (int)layer;
    }

    void TextureRenderer::setPosition(CoordinateX coordinateX, CoordinateY coordinateY) {
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

    void TextureRenderer::initialize(std::string name, RenderTarget& renderTarget, unsigned int sceneWidth, unsigned int sceneHeight,
                                     float minColorRange, float maxColorRange) {
        if (isInitialized) {
            throw std::runtime_error("Texture displayer cannot be initialized twice.");
        }

        this->renderingData.minColorRange = minColorRange;
        this->renderingData.maxColorRange = maxColorRange;

        initializeShader(renderTarget.isTestMode());

        float minX;
        float maxX;
        float minY;
        float maxY;
        if (fullScreen) {
            minX = 0.0f;
            maxX = (float)sceneWidth;
            minY = 0.0f;
            maxY = (float)sceneHeight;
        } else {
            switch(coordinateX) {
                case LEFT:
                    minX = (float)sceneWidth * 0.025f;
                    maxX = (float)sceneWidth * 0.325f;
                    break;
                case CENTER_X:
                    minX = (float)sceneWidth * 0.35f;
                    maxX = (float)sceneWidth * 0.65f;
                    break;
                case RIGHT:
                    minX = (float)sceneWidth * 0.675f;
                    maxX = (float)sceneWidth * 0.975f;
                    break;
                case USER_DEFINED_X:
                    minX = userMinX;
                    maxX = userMaxX;
                    break;
                default:
                    throw std::domain_error("Unsupported coordinate X");
            }

            switch(coordinateY) {
                case BOTTOM:
                    minY = (float)sceneHeight * 0.675f;
                    maxY = (float)sceneHeight * 0.975f;
                    break;
                case CENTER_Y:
                    minY = (float)sceneHeight * 0.35f;
                    maxY = (float)sceneHeight * 0.65f;
                    break;
                case TOP:
                    minY = (float)sceneHeight * 0.025f;
                    maxY = (float)sceneHeight * 0.325f;
                    break;
                case USER_DEFINED_Y:
                    minY = userMinY;
                    maxY = userMaxY;
                    break;
                default:
                    throw std::domain_error("Unsupported coordinate Y");
            }
        }

        //orthogonal matrix with origin at top left screen
        mProjection.setValues(2.0f / (float)sceneWidth, 0.0f, -1.0f, 0.0f,
                              0.0f, 2.0f / (float)sceneHeight, -1.0f, 0.0f,
                              0.0f, 0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f);

        //update the display
        std::vector vertexCoord = {
                Point2(minX, minY), Point2(maxX, minY), Point2(maxX, maxY),
                Point2(minX, minY), Point2(maxX, maxY), Point2(minX, maxY)
        };
        std::vector textureCoord = {
                Point2(0.0f, 0.0f), Point2(1.0f, 0.0f), Point2(1.0f, 1.0f),
                Point2(0.0f, 0.0f), Point2(1.0f, 1.0f), Point2(0.0f, 1.0f)
        };
        auto rendererBuilder = GenericRendererBuilder::create(std::move(name), renderTarget, *displayTextureShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformData(PROJ_MATRIX_UNIFORM_BINDING, sizeof(mProjection), &mProjection)
                ->addUniformData(RENDERING_DATA_UNIFORM_BINDING, sizeof(renderingData), &renderingData)
                ->addUniformTextureReader(TEX_UNIFORM_BINDING, TextureReader::build(texture, TextureParam::buildNearest()));
        if (transparencyEnabled) {
            rendererBuilder->enableTransparency({BlendFunction::buildDefault()});
        }
        renderer = rendererBuilder->build();

        isInitialized = true;
    }

    void TextureRenderer::initializeShader(bool isTestMode) {
        TextureRendererShaderConst trConstData {
            .isDefaultValue = colorType == DEFAULT_VALUE,
            .isGrayscaleValue = colorType == GRAYSCALE_VALUE,
            .isInverseGrayscaleValue = colorType == INVERSE_GRAYSCALE_VALUE
        };
        std::vector variablesSize = {
                sizeof(TextureRendererShaderConst::isDefaultValue),
                sizeof(TextureRendererShaderConst::isGrayscaleValue),
                sizeof(TextureRendererShaderConst::isInverseGrayscaleValue)
        };
        auto shaderConstants = std::make_unique<ShaderConstants>(variablesSize, &trConstData);

        std::string fragShaderName = (renderingData.layer == -1) ? "displayTexture.frag.spv" : "displayTextureArray.frag.spv";
        displayTextureShader = ShaderBuilder::createShader("displayTexture.vert.spv", fragShaderName, std::move(shaderConstants), isTestMode);
    }

    void TextureRenderer::prepareRendering(unsigned int renderingOrder) const {
        if (!isInitialized) {
            throw std::runtime_error("Texture displayer must be initialized before prepare rendering.");
        }

        renderer->enableRenderer(renderingOrder);
    }

}
