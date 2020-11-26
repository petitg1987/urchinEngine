#include "TextureReader.h"

namespace urchin {

    TextureReader::TextureReader(std::shared_ptr<Texture> texture, TextureParam param) :
            texture(std::move(texture)),
            param(param) {

    }

    TextureReader TextureReader::build(std::shared_ptr<Texture> texture, TextureParam textureParam) {
        return TextureReader(std::move(texture), textureParam);
    }

    unsigned int TextureReader::getId() const {
        return texture->getTextureId();
    }

    TextureType TextureReader::getType() const {
        return texture->getTextureType();
    }

    TextureParam TextureReader::getParam() const {
        return param;
    }

}
