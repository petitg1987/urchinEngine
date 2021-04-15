#include <stdexcept>

#include "TextureFilter.h"
#include "graphic/render/shader/builder/ShaderBuilder.h"
#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    TextureFilter::~TextureFilter() {
        if (offscreenRenderTarget) {
            offscreenRenderTarget->cleanup();
        }
    }

    void TextureFilter::initialize(const std::string& filterName) {
        if (isInitialized) {
            throw std::runtime_error("Texture filter is already initialized");
        }

        initializeTexture(filterName);
        initializeDisplay(filterName);

        isInitialized = true;
    }

    void TextureFilter::initializeTexture(const std::string& filterName) {
        if (textureType == TextureType::DEFAULT) {
            texture = Texture::build(textureWidth, textureHeight, textureFormat, nullptr);
        } else if (textureType == TextureType::ARRAY) {
            texture = Texture::buildArray(textureWidth, textureHeight, textureNumberLayer, textureFormat, nullptr);
        } else {
            throw std::invalid_argument("Unsupported texture type for filter: " + std::to_string(textureType));
        }

        offscreenRenderTarget = std::make_unique<OffscreenRender>(filterName, RenderTarget::NO_DEPTH_ATTACHMENT);
        offscreenRenderTarget->addTexture(texture);
        offscreenRenderTarget->initialize();
    }

    void TextureFilter::initializeDisplay(const std::string& filterName) {
        std::unique_ptr<ShaderConstants> shaderConstants = buildShaderConstants();

        if (textureType == TextureType::ARRAY) {
            textureFilterShader = ShaderBuilder::createShader("spirv/texFilter.vert.spv", "spirv/texFilter.geom.spv", "spirv/" + getShaderName() + ".frag.spv", std::move(shaderConstants));
        } else if (textureType == TextureType::DEFAULT) {
            textureFilterShader = ShaderBuilder::createShader("spirv/texFilter.vert.spv", "", "spirv/" + getShaderName() + ".frag.spv", std::move(shaderConstants));
        } else {
            throw std::invalid_argument("Unsupported texture type for filter: " + std::to_string(textureType));
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
        auto textureRendererBuilder = GenericRendererBuilder::create(filterName, offscreenRenderTarget, textureFilterShader, ShapeType::TRIANGLE)
                ->addData(vertexCoord)
                ->addData(textureCoord)
                ->addShaderData(sizeof(layersToUpdate), &layersToUpdate) //binding 0
                ->addTextureReader(TextureReader::build(sourceTexture, TextureParam::buildLinear()));
        completeRenderer(textureRendererBuilder);

        textureRenderer = textureRendererBuilder->build();
    }

    void TextureFilter::completeRenderer(const std::shared_ptr<GenericRendererBuilder>&) {
        //do nothing: to override
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

    const std::shared_ptr<Shader>& TextureFilter::getTextureFilterShader() const {
        return textureFilterShader;
    }

    const std::shared_ptr<GenericRenderer>& TextureFilter::getTextureRenderer() const {
        return textureRenderer;
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
            if (layersToUpdate != 0) {
                //TODO if all layers are not updated: we should ensure color attachment is not clear (see  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR)
                textureRenderer->updateShaderData(0, &layersToUpdate);
                offscreenRenderTarget->render();
            }
        } else {
            offscreenRenderTarget->render();
        }
    }
}
