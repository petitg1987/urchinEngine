#include <UrchinCommon.h>

#include <loader/texture/LoaderTexture.h>
#include <resources/ResourceRetriever.h>

namespace urchin {

    std::shared_ptr<Texture> LoaderTexture::loadFromFile(const std::string& filename, const std::map<std::string, std::string, std::less<>>& params) {
        assert(params.find("mipMap") != params.end());
        unsigned int generateMipMap = TypeConverter::toBool(params.find("mipMap")->second);

        auto image = ResourceRetriever::instance().getResource<Image>(filename);

        std::shared_ptr<Texture> texture;
        if (image->getChannelPrecision() == Image::CHANNEL_8) {
            texture = Texture::build(image->getName(), image->getWidth(), image->getHeight(), image->retrieveTextureFormat(), image->getTexels().data());
        }else if (image->getChannelPrecision() == Image::CHANNEL_16) {
            texture = Texture::build(image->getName(), image->getWidth(), image->getHeight(), image->retrieveTextureFormat(), image->getTexels16Bits().data());
        } else {
            throw std::runtime_error("Unknown channel precision: " + std::to_string(image->getChannelPrecision()));
        }

        texture->setHasTransparency(image->hasTransparency());
        if (generateMipMap) {
            texture->enableMipmap();
        }

        return texture;
    }

}
