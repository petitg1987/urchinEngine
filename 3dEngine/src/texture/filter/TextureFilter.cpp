#include <stdexcept>

#include "TextureFilter.h"
#include "graphic/shader/builder/ShaderBuilder.h"
#include "graphic/shader/data/ShaderDataSender.h"
#include "graphic/render/GenericRendererBuilder.h"

namespace urchin {

    void TextureFilter::initialize() {
        if (isInitialized) {
            throw std::runtime_error("Texture filter is already initialized");
        }

        initializeDisplay();
        initializeTexture();

        isInitialized = true;
    }

    void TextureFilter::initializeDisplay() {
        std::locale::global(std::locale("C")); //for float

        std::vector<Point2<float>> vertexCoord = {
                Point2<float>(-1.0f, 1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.0f, -1.0f),
                Point2<float>(-1.0f, 1.0f), Point2<float>(1.0f, -1.0f), Point2<float>(-1.0f, -1.0f)
        };
        std::vector<Point2<float>> textureCoord = {
                Point2<float>(0.0f, 1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.0f, 0.0f),
                Point2<float>(0.0f, 1.0f), Point2<float>(1.0f, 0.0f), Point2<float>(0.0f, 0.0f)
        };
        textureRenderer = std::make_unique<GenericRendererBuilder>(ShapeType::TRIANGLE)
                ->addData(&vertexCoord)
                ->addData(&textureCoord)
                ->build();

        std::map<std::string, std::string> shaderTokens;
        if (textureFormat == TextureFormat::RGB_8_INT) {
            shaderTokens["OUTPUT_TYPE"] = "vec3";
            shaderTokens["SOURCE_TEX_COMPONENTS"] = "rgb";
        } else if (textureFormat == TextureFormat::RG_32_FLOAT) {
            shaderTokens["OUTPUT_TYPE"] = "vec2";
            shaderTokens["SOURCE_TEX_COMPONENTS"] = "rg";
        } else if (textureFormat == TextureFormat::GRAYSCALE_16_FLOAT) {
            shaderTokens["OUTPUT_TYPE"] = "float";
            shaderTokens["SOURCE_TEX_COMPONENTS"] = "r";
        } else {
            throw std::invalid_argument("Unsupported texture format for filter: " + std::to_string(textureFormat));
        }

        this->completeShaderTokens(shaderTokens);

        if (textureType == TextureType::ARRAY) {
            shaderTokens["MAX_VERTICES"] = std::to_string(3 * textureNumberLayer);
            shaderTokens["NUMBER_LAYER"] = std::to_string(textureNumberLayer);

            textureFilterShader = ShaderBuilder().createShader("textureFilter.vert", "textureFilter.geom", getShaderName() + "Array.frag", shaderTokens);
        } else if (textureType == TextureType::DEFAULT) {
            textureFilterShader = ShaderBuilder().createShader("textureFilter.vert", "", getShaderName() + ".frag", shaderTokens);
        } else {
            throw std::invalid_argument("Unsupported texture type for filter: " + std::to_string(textureType));
        }

        int texUnit = 0;
        ShaderDataSender().sendData(ShaderVar(textureFilterShader, "tex"), texUnit);
        layersToUpdateShaderVar = ShaderVar(textureFilterShader, "layersToUpdate");
        initiateAdditionalShaderVariables(textureFilterShader);
    }

    void TextureFilter::initializeTexture() {
        if (textureType == TextureType::DEFAULT) {
            texture = Texture::build(textureWidth, textureHeight, textureFormat, nullptr);
        } else if( textureType == TextureType::ARRAY) {
            texture = Texture::buildArray(textureWidth, textureHeight, textureNumberLayer, textureFormat, nullptr);
        } else {
            throw std::invalid_argument("Unsupported texture type for filter: " + std::to_string(textureType));
        }

        offscreenRenderTarget = std::make_unique<OffscreenRender>();
        offscreenRenderTarget->onResize(textureWidth, textureHeight);
        offscreenRenderTarget->addTexture(texture);
    }

    void TextureFilter::initiateAdditionalShaderVariables(const std::unique_ptr<Shader> &) {
        //do nothing: to override
    }

    void TextureFilter::addFurtherTextures(const std::unique_ptr<GenericRenderer> &) const {
        //do nothing: to override
    }

    const std::shared_ptr<Texture> &TextureFilter::getTexture() const {
        return texture;
    }

    unsigned int TextureFilter::getTextureWidth() const {
        return textureWidth;
    }

    unsigned int TextureFilter::getTextureHeight() const {
        return textureHeight;
    }

    const std::unique_ptr<Shader> &TextureFilter::getTextureFilterShader() const {
        return textureFilterShader;
    }

    std::string TextureFilter::toShaderVectorValues(std::vector<float>& vector) const {
        std::string vectorValuesStr;
        for (std::size_t i=0;i<vector.size(); ++i) {
            vectorValuesStr += std::to_string(vector[i]);
            if (i!=vector.size()-1) {
                vectorValuesStr += ", ";
            }
        }

        return vectorValuesStr;
    }

    /**
     * @param layersToUpdate Specify the layers which must be affected by the filter (only for TextureFormat::ARRAY).
     * Lowest bit represent the first layer, the second lowest bit represent the second layer, etc.
     */
    void TextureFilter::applyOn(const std::shared_ptr<Texture>& sourceTexture, int layersToUpdate) const {
        assert(layersToUpdate == -1 || sourceTexture->getTextureType() == TextureType::ARRAY);
        if (!isInitialized) {
            throw std::runtime_error("Texture filter must be initialized before apply.");
        }

        textureRenderer->clearAdditionalTextures();
        textureRenderer->addAdditionalTexture(TextureReader::build(sourceTexture, TextureParam::buildLinear()));
        addFurtherTextures(textureRenderer);

        if (textureType == TextureType::ARRAY) {
            ShaderDataSender().sendData(layersToUpdateShaderVar, static_cast<unsigned int>(layersToUpdate));
        }

        offscreenRenderTarget->activeShader(textureFilterShader);
        offscreenRenderTarget->display(textureRenderer);
    }
}
