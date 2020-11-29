#include "TextureReader.h"

namespace urchin {

    TextureReader::TextureReader(std::shared_ptr<Texture> texture, TextureParam param) :
            texture(std::move(texture)),
            param(param) {

    }

    TextureReader TextureReader::build(std::shared_ptr<Texture> texture, TextureParam textureParam) {
        return TextureReader(std::move(texture), textureParam);
    }

    const std::shared_ptr<Texture>& TextureReader::getTexture() const {
        return texture;
    }

    TextureParam TextureReader::getParam() const {
        return param;
    }

}
