#include <stdexcept>
#include <utility>

#include <texture/render/TextureRenderer.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/GenericRendererBuilder.h>

namespace urchin {

    TextureRenderer::TextureRenderer(std::shared_ptr<Texture> texture, TextureRenderer::ColorType colorType) :
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
            renderingData({}) {
        renderingData.minColorRange = 0.0f;
        renderingData.maxColorRange = 1.0f;
        renderingData.layer = -1;
    }

    TextureRenderer::TextureRenderer(std::shared_ptr<Texture> texture, unsigned int layer, TextureRenderer::ColorType colorType) :
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
            renderingData({}) {
        assert(this->texture->getTextureType() == TextureType::ARRAY);

        renderingData.minColorRange = 0.0f;
        renderingData.maxColorRange = 1.0f;
        renderingData.layer = (int)layer;
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

    void TextureRenderer::initialize(std::string name, RenderTarget& renderTarget, unsigned int sceneWidth, unsigned int sceneHeight,
                                     float minColorRange, float maxColorRange) {
        if (isInitialized) {
            throw std::runtime_error("Texture displayer cannot be initialized twice.");
        }

        this->renderingData.minColorRange = minColorRange;
        this->renderingData.maxColorRange = maxColorRange;

        initializeShader();

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
        mProjection.setValues(2.0f / (float)sceneWidth, 0.0f, -1.0f, 0.0f,
                              0.0f, 2.0f / (float)sceneHeight, -1.0f, 0.0f,
                              0.0f, 0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f);

        //update the display
        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(minX, minY), Point2<float>(maxX, minY), Point2<float>(maxX, maxY),
                Point2<float>(minX, minY), Point2<float>(maxX, maxY), Point2<float>(minX, maxY)
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
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

    void TextureRenderer::initializeShader() {
        TextureRendererShaderConst trConstData{};
        trConstData.isDefaultValue = colorType == ColorType::DEFAULT_VALUE;
        trConstData.isGrayscaleValue = colorType == ColorType::GRAYSCALE_VALUE;
        trConstData.isInverseGrayscaleValue = colorType == ColorType::INVERSE_GRAYSCALE_VALUE;
        std::vector<std::size_t> variablesSize = {
                sizeof(TextureRendererShaderConst::isDefaultValue),
                sizeof(TextureRendererShaderConst::isGrayscaleValue),
                sizeof(TextureRendererShaderConst::isInverseGrayscaleValue)
        };
        auto shaderConstants = std::make_unique<ShaderConstants>(variablesSize, &trConstData);

        std::string fragShaderName = (renderingData.layer == -1) ? "displayTexture.frag.spv" : "displayTextureArray.frag.spv";
        displayTextureShader = ShaderBuilder::createShader("displayTexture.vert.spv", fragShaderName, std::move(shaderConstants));
    }

    void TextureRenderer::prepareRendering(unsigned int renderingOrder) const {
        if (!isInitialized) {
            throw std::runtime_error("Texture displayer must be initialized before prepare rendering.");
        }

        renderer->enableRenderer(renderingOrder);
    }

}
