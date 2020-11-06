#include <GL/glew.h>
#include <stdexcept>

#include "TextureParam.h"

namespace urchin {

    TextureParam::TextureParam(ReadMode readMode, ReadQuality readQuality) :
        readMode(readMode),
        readQuality(readQuality) {

    }

    TextureParam TextureParam::buildNearest() {
        return build(ReadMode::EDGE_CLAMP, ReadQuality::NEAREST);
    }

    TextureParam TextureParam::buildLinear() {
        return build(ReadMode::EDGE_CLAMP, ReadQuality::LINEAR);
    }

    TextureParam TextureParam::build(ReadMode readMode, ReadQuality readQuality) {
        return TextureParam(readMode, readQuality);
    }

    unsigned int TextureParam::getGlReadMode() const {
        if (readMode == TextureParam::EDGE_CLAMP) {
            return GL_CLAMP_TO_EDGE;
        }else if(readMode == TextureParam::REPEAT) {
            return GL_REPEAT;
        }
        throw std::runtime_error("Unknown texture read mode: " + std::to_string(readMode));
    }

    unsigned int TextureParam::getGlReadQuality() const {
        if (readQuality == TextureParam::NEAREST) {
            return GL_NEAREST;
        }else if(readQuality == TextureParam::LINEAR) {
            return GL_LINEAR;
        }
        throw std::runtime_error("Unknown texture read quality: " + std::to_string(readQuality));
    }

}
