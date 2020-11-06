#include <GL/glew.h>
#include <stdexcept>

#include "Texture.h"

namespace urchin {

    Texture::Texture(unsigned int id, Type type, TextureParam param) :
        id(id),
        type(type),
        param(param) {

    }

    Texture Texture::build(unsigned int id) {
        return Texture(id, Type::DEFAULT, TextureParam::buildNearest());
    }

    Texture Texture::build(unsigned int id, Type type) {
        return Texture(id, type, TextureParam::buildNearest());
    }

    Texture Texture::build(unsigned int id, Type type, TextureParam textureParam) {
        return Texture(id, type, textureParam);
    }

    unsigned int Texture::getId() const {
        return id;
    }

    unsigned int Texture::getGlType() const {
        if (type == Texture::DEFAULT) {
            return GL_TEXTURE_2D;
        } else if (type == Texture::ARRAY) {
            return GL_TEXTURE_2D_ARRAY;
        } else if (type == Texture::CUBE_MAP) {
            return GL_TEXTURE_CUBE_MAP;
        }
        throw std::runtime_error("Unknown texture type: " + std::to_string(type));
    }

    TextureParam Texture::getParam() const {
        return param;
    }

}
