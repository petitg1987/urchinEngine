#include <graphic/texture/TextureReader.h>

namespace urchin {

    TextureReader::TextureReader(std::shared_ptr<Texture> texture, const TextureParam& param) :
            isInitialized(false),
            texture(std::move(texture)),
            param(param) {

    }

    TextureReader::~TextureReader() {
        cleanup();
    }

    std::shared_ptr<TextureReader> TextureReader::build(std::shared_ptr<Texture> texture, const TextureParam& textureParam) {
        return std::shared_ptr<TextureReader>(new TextureReader(std::move(texture), textureParam));
    }

    void TextureReader::initialize() {
        if(!isInitialized) {
            texture->initialize();
            param.initialize(texture->getMipLevels());

            isInitialized = true;
        }
    }

    void TextureReader::cleanup() {
        if(isInitialized) {
            param.cleanup();
            texture = nullptr;

            isInitialized = false;
        }
    }

    const std::shared_ptr<Texture>& TextureReader::getTexture() const {
        return texture;
    }

    const TextureParam& TextureReader::getParam() const {
        return param;
    }

}
