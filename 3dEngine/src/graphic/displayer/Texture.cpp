#include "Texture.h"

namespace urchin {

    Texture::Texture(unsigned int id, Type type) :
        id(id),
        type(type) {

    }

    Texture Texture::build(unsigned int id) {
        return Texture(id, Type::SIMPLE);
    }

    Texture Texture::build(unsigned int id, Type type) {
        return Texture(id, type);
    }

    unsigned int Texture::getId() const {
        return id;
    }

    Texture::Type Texture::getType() const {
        return type;
    }

}
