#include "TextureReader.h"

namespace urchin {

    TextureReader::TextureReader(unsigned int id, TextureType type, TextureParam param) :
        id(id),
        type(type),
        param(param) {

    }

    TextureReader TextureReader::build(unsigned int id) {
        return TextureReader(id, TextureType::DEFAULT, TextureParam::buildNearest());
    }

    TextureReader TextureReader::build(unsigned int id, TextureType type) {
        return TextureReader(id, type, TextureParam::buildNearest());
    }

    TextureReader TextureReader::build(unsigned int id, TextureType type, TextureParam textureParam) {
        return TextureReader(id, type, textureParam);
    }

    unsigned int TextureReader::getId() const {
        return id;
    }

    TextureType TextureReader::getType() const {
        return type;
    }

    TextureParam TextureReader::getParam() const {
        return param;
    }

}
