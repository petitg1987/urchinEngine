#include <GL/glew.h>
#include <stdexcept>
#include <cassert>

#include "Texture.h"

namespace urchin {

    Texture::Texture(TextureType textureType, unsigned int width, unsigned int height, unsigned int layer, TextureFormat format, const std::vector<const void *> &dataPtr) :
            textureType(textureType),
            width(width),
            height(height),
            layer(layer),
            format(format),
            textureId(0) {
        unsigned int glTextureType = textureTypeToGlTextureType(textureType);
        unsigned int glInternalFormat = textureFormatToGlInternalFormat(format);
        unsigned int glFormat = textureFormatToGlFormat(format);
        unsigned int glPixelType = textureFormatToGlPixelType(format);

        glGenTextures(1, &textureId);
        glBindTexture(glTextureType, textureId);

        if (textureType == TextureType::DEFAULT) {
            assert(layer == 1);
            assert(dataPtr.size() == 1);

            glTexImage2D(glTextureType, 0, glInternalFormat, width, height, 0, glFormat, glPixelType, dataPtr[0]);
        } else if (textureType == TextureType::ARRAY) {
            assert(layer > 1);
            assert(dataPtr.size() == 1);

            glTexImage3D(glTextureType, 0, glInternalFormat, width, height, layer, 0, glFormat, glPixelType, dataPtr[0]);
        } else if (textureType == TextureType::CUBE_MAP) {
            assert(layer == 1);
            assert(dataPtr.size() == 6);

            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, glInternalFormat, width, height, 0, glFormat, glPixelType, dataPtr[0]);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, glInternalFormat, width, height, 0, glFormat, glPixelType, dataPtr[1]);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, glInternalFormat, width, height, 0, glFormat, glPixelType, dataPtr[2]);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, glInternalFormat, width, height, 0, glFormat, glPixelType, dataPtr[3]);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, glInternalFormat, width, height, 0, glFormat, glPixelType, dataPtr[4]);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, glInternalFormat, width, height, 0, glFormat, glPixelType, dataPtr[5]);
        }
    }

    Texture::~Texture() {
        glDeleteTextures(1, &textureId);
    }

    std::shared_ptr<Texture> Texture::build(unsigned int width, unsigned int height, TextureFormat format, const void *dataPtr) {
        std::vector<const void *> allDataPtr = {dataPtr};
        return std::make_shared<Texture>(TextureType::DEFAULT, width, height, 1, format, allDataPtr);
    }

    std::shared_ptr<Texture> Texture::buildArray(unsigned int width, unsigned int height, unsigned int layer, TextureFormat format, const void *dataPtr) {
        std::vector<const void *> allDataPtr = {dataPtr};
        return std::make_shared<Texture>(TextureType::ARRAY, width, height, layer, format, allDataPtr);
    }

    std::shared_ptr<Texture> Texture::buildCubeMap(unsigned int width, unsigned int height, TextureFormat format, const std::vector<const void *> &cubeDataPtr) {
        return std::make_shared<Texture>(TextureType::CUBE_MAP, width, height, 1, format, cubeDataPtr);
    }

    void Texture::generateMipmap() const {
        unsigned int glTextureType = textureTypeToGlTextureType(textureType);

        glBindTexture(glTextureType, textureId);
        glGenerateMipmap(glTextureType);
    }

    TextureType Texture::getTextureType() const {
        return textureType;
    }

    unsigned int Texture::getWidth() const {
        return width;
    }

    unsigned int Texture::getHeight() const {
        return height;
    }

    unsigned int Texture::getTextureId() const {
        return textureId;
    }

    unsigned int Texture::textureTypeToGlTextureType(TextureType textureType) const {
        if (textureType == TextureType::DEFAULT) {
            return GL_TEXTURE_2D;
        } else if (textureType == TextureType::ARRAY) {
            return GL_TEXTURE_2D_ARRAY;
        } else if (textureType == TextureType::CUBE_MAP) {
            return GL_TEXTURE_CUBE_MAP;
        }
        throw std::runtime_error("Unknown texture type: " + std::to_string(textureType));
    }

    unsigned int Texture::textureFormatToGlInternalFormat(TextureFormat textureFormat) const {
        if(textureFormat == TextureFormat::DEPTH_16_FLOAT) {
            return GL_DEPTH_COMPONENT16;
        } else if(textureFormat == TextureFormat::DEPTH_24_FLOAT) {
            return GL_DEPTH_COMPONENT24;
        } else if (textureFormat == TextureFormat::DEPTH_32_FLOAT) {
            return GL_DEPTH_COMPONENT32;
        } else if (textureFormat == TextureFormat::GRAYSCALE_8_INT) {
            return GL_RED;
        } else if (textureFormat == TextureFormat::GRAYSCALE_16_FLOAT) {
            return GL_R16F;
        }else if (textureFormat == TextureFormat::RG_32_FLOAT) {
            return GL_RG32F;
        } else if (textureFormat == TextureFormat::RGB_8_INT) {
            return GL_RGB;
        } else if (textureFormat == TextureFormat::RGB_16_FLOAT) {
            return GL_RGB16F;
        } else if (textureFormat == TextureFormat::RGBA_8_INT) {
            return GL_RGBA;
        } else {
            throw std::runtime_error("Unknown texture format: " + std::to_string(format));
        }
    }

    unsigned int Texture::textureFormatToGlFormat(TextureFormat textureFormat) const {
        if(textureFormat == TextureFormat::DEPTH_16_FLOAT || textureFormat == TextureFormat::DEPTH_24_FLOAT || textureFormat == TextureFormat::DEPTH_32_FLOAT) {
            return GL_DEPTH_COMPONENT;
        } else if (textureFormat == TextureFormat::GRAYSCALE_8_INT || textureFormat == TextureFormat::GRAYSCALE_16_FLOAT) {
            return GL_RED;
        } else if (textureFormat == TextureFormat::RG_32_FLOAT) {
            return GL_RG;
        } else if (textureFormat == TextureFormat::RGB_8_INT || textureFormat == TextureFormat::RGB_16_FLOAT) {
            return GL_RGB;
        } else if (textureFormat == TextureFormat::RGBA_8_INT) {
            return GL_RGBA;
        } else {
            throw std::runtime_error("Unknown texture format: " + std::to_string(format));
        }
    }

    unsigned int Texture::textureFormatToGlPixelType(TextureFormat textureFormat) const {
        if (textureFormat == DEPTH_16_FLOAT
                || textureFormat == DEPTH_24_FLOAT
                || textureFormat == DEPTH_32_FLOAT
                || textureFormat == GRAYSCALE_16_FLOAT
                || textureFormat == RG_32_FLOAT
                || textureFormat == RGB_16_FLOAT) {
            return GL_FLOAT;
        } else if (textureFormat == GRAYSCALE_8_INT
                || textureFormat == RGB_8_INT
                || textureFormat == RGBA_8_INT) {
            return GL_UNSIGNED_BYTE;
        } else {
            throw std::runtime_error("Unknown texture format: " + std::to_string(format));
        }
    }

}
