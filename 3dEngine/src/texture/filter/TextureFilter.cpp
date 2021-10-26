#include <stdexcept>

#include <texture/filter/TextureFilter.h>
#include <api/render/shader/builder/ShaderBuilder.h>
#include <api/render/GenericRendererBuilder.h>
#include <api/render/target/NullRenderTarget.h>

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
            texture = Texture::build(textureWidth, textureHeight, textureFormat, nullptr);
        } else if (textureType == TextureType::ARRAY) {
            texture = Texture::buildArray(textureWidth, textureHeight, textureNumberLayer, textureFormat, nullptr);
        } else {
            throw std::invalid_argument("Unsupported texture type for filter: " + std::to_string((int)textureType));
        }

        if (useNullRenderTarget) {
            renderTarget = std::make_unique<NullRenderTarget>(texture->getWidth(), texture->getHeight());
        } else {
            renderTarget = std::make_unique<OffscreenRender>(name, RenderTarget::NO_DEPTH_ATTACHMENT);
            dynamic_cast<OffscreenRender*>(renderTarget.get())->addOutputTexture(texture);
            renderTarget->initialize();
        }
    }

    void TextureFilter::initializeDisplay() {
        assert(renderTarget);
        std::unique_ptr<ShaderConstants> shaderConstants = buildShaderConstants();

        if (useNullRenderTarget) {
            textureFilterShader = ShaderBuilder::createNullShader();
        } else {
            if (textureType == TextureType::ARRAY) {
                textureFilterShader = ShaderBuilder::createShader("texFilter.vert.spv", "texFilter.geom.spv", getShaderName() + ".frag.spv", std::move(shaderConstants));
            } else if (textureType == TextureType::DEFAULT) {
                textureFilterShader = ShaderBuilder::createShader("texFilter.vert.spv", "", getShaderName() + ".frag.spv", std::move(shaderConstants));
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
        int layersToUpdate = 0;
        auto textureRendererBuilder = GenericRendererBuilder::create(name, *renderTarget, *textureFilterShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addUniformData(sizeof(layersToUpdate), &layersToUpdate); //binding 0
        auto sourceTargetReader = TextureReader::build(sourceTexture, TextureParam::buildLinear());
        completeRenderer(textureRendererBuilder, sourceTargetReader);

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

    unsigned int TextureFilter::getTextureLayer() const {
        return textureNumberLayer;
    }

    const Shader& TextureFilter::getTextureFilterShader() const {
        assert(textureFilterShader);
        return *textureFilterShader;
    }

    GenericRenderer& TextureFilter::getTextureRenderer() const {
        return *textureRenderer;
    }

    /**
     * @param layersToUpdate Specify the layers which must be affected by the filter (only for TextureFormat::ARRAY).
     * Lowest bit represent the first layer, the second lowest bit represent the second layer, etc.
     */
    void TextureFilter::applyFilter(int layersToUpdate) const {
        if (!isInitialized) {
            throw std::runtime_error("Texture filter must be initialized before apply.");
        }

        if (textureType == TextureType::ARRAY) {
            textureRenderer->updateUniformData(0, &layersToUpdate);
        }
        renderTarget->render();
    }
}
