#include <GL/glew.h>
#include <stdexcept>

#include "TextureParam.h"

namespace urchin {

    TextureParam::TextureParam(ReadMode readMode, ReadQuality readQuality, Anisotropy anisotropy) :
        readMode(readMode),
        readQuality(readQuality),
        anisotropy(anisotropy) {

    }

    TextureParam TextureParam::buildNearest() {
        return TextureParam(ReadMode::EDGE_CLAMP, ReadQuality::NEAREST, Anisotropy::NO_ANISOTROPY);
    }

    TextureParam TextureParam::buildLinear() {
        return TextureParam(ReadMode::EDGE_CLAMP, ReadQuality::LINEAR, Anisotropy::NO_ANISOTROPY);
    }

    TextureParam TextureParam::buildRepeatNearest() {
        return TextureParam(ReadMode::REPEAT, ReadQuality::NEAREST, Anisotropy::NO_ANISOTROPY);
    }

    TextureParam TextureParam::buildRepeatLinear() {
        return TextureParam(ReadMode::REPEAT, ReadQuality::LINEAR, Anisotropy::NO_ANISOTROPY);
    }

    TextureParam TextureParam::build(ReadMode readMode, ReadQuality readQuality, Anisotropy anisotropy) {
        return TextureParam(readMode, readQuality, anisotropy);
    }

    unsigned int TextureParam::getGlReadMode() const {
        if (readMode == ReadMode::EDGE_CLAMP) {
            return GL_CLAMP_TO_EDGE;
        } else if (readMode == ReadMode::REPEAT) {
            return GL_REPEAT;
        }
        throw std::runtime_error("Unknown texture read mode: " + std::to_string(readMode));
    }

    unsigned int TextureParam::getGlReadQuality(bool forMipmap) const {
        if (readQuality == ReadQuality::NEAREST && !forMipmap) {
            return GL_NEAREST;
        } else if (readQuality == ReadQuality::LINEAR && !forMipmap) {
            return GL_LINEAR;
        } else if (readQuality == ReadQuality::NEAREST && forMipmap) {
            return GL_NEAREST_MIPMAP_NEAREST;
        } else if (readQuality == ReadQuality::LINEAR && forMipmap) {
            return GL_LINEAR_MIPMAP_LINEAR;
        }
        throw std::runtime_error("Unknown texture read quality: " + std::to_string(readQuality));
    }

    bool TextureParam::needAnisotropy() const {
        if (anisotropy == Anisotropy::NO_ANISOTROPY) {
            return false;
        } else if (anisotropy == Anisotropy::ANISOTROPY) {
            return true;
        }
        throw std::runtime_error("Unknown anisotropy type: " + std::to_string(anisotropy));
    }

}
