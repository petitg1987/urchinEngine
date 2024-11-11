#include <UrchinCommon.h>

#include <loader/texture/LoaderTexture.h>
#include <resources/ResourceRetriever.h>
#include <resources/image/Image.h>

namespace urchin {

    //static
    const std::string LoaderTexture::DEFAULT_TEXTURE_FILENAME = std::string(1, ResourceRetriever::SPECIAL_FILENAME_PREFIX) + "DEFAULT_TEXTURE";

    std::shared_ptr<Texture> LoaderTexture::loadFromFile(const std::string& filename, const std::map<std::string, std::string, std::less<>>& params) {
        if (filename == DEFAULT_TEXTURE_FILENAME) {
            std::string defaultTextureType = params.at("textureType");
            if (defaultTextureType == "normal") {
                std::vector<unsigned char> defaultNormalColor({127, 127, 255, 255});
                return Texture::build("defaultNormal", 1, 1, TextureFormat::RGBA_8_INT, defaultNormalColor.data(), TextureDataType::INT_8);
            } else if (defaultTextureType == "roughness") {
                std::vector<unsigned char> defaultRoughnessValue({255});
                return Texture::build("defaultRoughness", 1, 1, TextureFormat::GRAYSCALE_8_INT, defaultRoughnessValue.data(), TextureDataType::INT_8);
            } else if (defaultTextureType == "metalness") {
                std::vector<unsigned char> defaultMetalnessValue({0});
                return Texture::build("defaultMetalness", 1, 1, TextureFormat::GRAYSCALE_8_INT, defaultMetalnessValue.data(), TextureDataType::INT_8);
            } else {
                 throw std::runtime_error("Unknown default texture type: " + defaultTextureType);
            }
        }

        auto image = ResourceRetriever::instance().getResource<Image>(filename);

        std::shared_ptr<Texture> texture;
        if (image->getChannelPrecision() == Image::CHANNEL_8_INT) {
            texture = Texture::build(image->getName(), image->getWidth(), image->getHeight(), image->retrieveTextureFormat(), image->getTexels().data(), TextureDataType::INT_8);
        }else if (image->getChannelPrecision() == Image::CHANNEL_16_INT) {
            texture = Texture::build(image->getName(), image->getWidth(), image->getHeight(), image->retrieveTextureFormat(), image->getTexels16Bits().data(), TextureDataType::INT_16);
        } else {
            throw std::runtime_error("Unknown channel precision: " + std::to_string(image->getChannelPrecision()));
        }

        texture->setHasTransparency(image->hasTransparency());
        unsigned int generateMipMap = TypeConverter::toBool(params.at("mipMap"));
        if (generateMipMap) {
            texture->enableMipmap();
        }

        return texture;
    }

}
