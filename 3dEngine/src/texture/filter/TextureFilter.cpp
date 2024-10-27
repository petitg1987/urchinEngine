#include <stdexcept>

#include <texture/filter/TextureFilter.h>
#include <graphics/render/shader/ShaderBuilder.h>
#include <graphics/render/GenericRendererBuilder.h>

namespace urchin {

    TextureFilter::~TextureFilter() {
        if (renderTarget) {
            renderTarget->cleanup();
        }
    }

    void TextureFilter::initialize() {
        if (isInitialized) {
            throw std::runtime_error("Texture filter is already initialized");
        }

        initializeTexture();
        initializeDisplay();

        isInitialized = true;
    }

    void TextureFilter::initializeTexture() {
        if (textureType == TextureType::DEFAULT) {
            texture = Texture::build(name, textureWidth, textureHeight, textureFormat);
        } else {
            throw std::invalid_argument("Unsupported texture type for filter: " + std::to_string((int)textureType));
        }

        renderTarget = std::make_unique<OffscreenRender>(name, useSimulationRenderTarget, RenderTarget::NO_DEPTH_ATTACHMENT);
        renderTarget->addOutputTexture(texture);
        renderTarget->initialize();
    }

    void TextureFilter::initializeDisplay() {
        assert(renderTarget);
        std::unique_ptr<ShaderConstants> shaderConstants = buildShaderConstants();

        if (useSimulationRenderTarget) {
            textureFilterShader = ShaderBuilder::createNullShader();
        } else {
            if (textureType == TextureType::DEFAULT) {
                textureFilterShader = ShaderBuilder::createShader("texFilter.vert.spv", getShaderName() + ".frag.spv", std::move(shaderConstants));
            } else {
                throw std::invalid_argument("Unsupported texture type for filter: " + std::to_string((int)textureType));
            }
        }

        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, -1.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(-1.0f, -1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(-1.0f, 1.0f)
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(1.0f, 1.0f),
                Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(0.0f, 1.0f)
        };
        auto textureRendererBuilder = GenericRendererBuilder::create(name, *renderTarget, *textureFilterShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord);
        auto sourceTextureReader = TextureReader::build(sourceTexture, TextureParam::buildNearest());
        completeRenderer(textureRendererBuilder, sourceTextureReader);

        textureRenderer = textureRendererBuilder->build();
    }

    const std::shared_ptr<Texture>& TextureFilter::getTexture() const {
        return texture;
    }

    TextureFormat TextureFilter::getTextureFormat() const {
        return textureFormat;
    }

    TextureType TextureFilter::getTextureType() const {
        return textureType;
    }

    unsigned int TextureFilter::getTextureWidth() const {
        return textureWidth;
    }

    unsigned int TextureFilter::getTextureHeight() const {
        return textureHeight;
    }

    const Shader& TextureFilter::getTextureFilterShader() const {
        assert(textureFilterShader);
        return *textureFilterShader;
    }

    GenericRenderer& TextureFilter::getTextureRenderer() const {
        return *textureRenderer;
    }

    void TextureFilter::applyFilter(std::uint32_t frameIndex, unsigned int numDependenciesToOutputTextures) const {
        if (!isInitialized) {
            throw std::runtime_error("Texture filter must be initialized before apply.");
        }
        renderTarget->render(frameIndex, numDependenciesToOutputTextures);
    }
}
