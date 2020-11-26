#include "TextureReader.h"

namespace urchin {

    TextureReader::TextureReader(unsigned int id, TextureType type, TextureParam param) :
            id(id),
            type(type),
            param(param) {

    }

    TextureReader::TextureReader(std::shared_ptr<Texture> texture, TextureParam param) :
            texture(std::move(texture)),
            id(this->texture->getTextureId()),
            type(this->texture->getTextureType()),
            param(param) {

    }

    TextureReader TextureReader::build(unsigned int id) {
        return TextureReader(id, TextureType::DEFAULT, TextureParam::buildNearest());
    }

    TextureReader TextureReader::build(std::shared_ptr<Texture> texture, TextureParam textureParam) {
        return TextureReader(std::move(texture), textureParam);
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
