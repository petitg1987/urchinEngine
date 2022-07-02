#include <graphics/api/vulkan/texture/TextureReader.h>

namespace urchin {

    TextureReader::TextureReader(std::shared_ptr<Texture> texture, TextureParam param) :
            isInitialized(false),
            texture(std::move(texture)),
            param(std::move(param)) {

    }

    TextureReader::~TextureReader() {
        cleanup();
    }

    std::shared_ptr<TextureReader> TextureReader::build(std::shared_ptr<Texture> texture, TextureParam textureParam) {
        return std::shared_ptr<TextureReader>(new TextureReader(std::move(texture), std::move(textureParam)));
    }

    void TextureReader::initialize() {
        if (!isInitialized) {
            texture->initialize();
            param.initialize(texture->getMipLevels());

            isInitialized = true;
        }
    }

    void TextureReader::cleanup() {
        if (isInitialized) {
            param.cleanup();
            texture = nullptr;

            isInitialized = false;
        }
    }

    const Texture* TextureReader::getTexture() const {
        return texture.get();
    }

    const TextureParam& TextureReader::getParam() const {
        return param;
    }

}
