#include "Texture.h"

namespace urchin {

    Texture::Texture(unsigned int id, TextureType type, TextureParam param) :
        id(id),
        type(type),
        param(param) {

    }

    Texture Texture::build(unsigned int id) {
        return Texture(id, TextureType::DEFAULT, TextureParam::buildNearest());
    }

    Texture Texture::build(unsigned int id, TextureType type) {
        return Texture(id, type, TextureParam::buildNearest());
    }

    Texture Texture::build(unsigned int id, TextureType type, TextureParam textureParam) {
        return Texture(id, type, textureParam);
    }

    unsigned int Texture::getId() const {
        return id;
    }

    TextureType Texture::getType() const {
        return type;
    }

    TextureParam Texture::getParam() const {
        return param;
    }

}
