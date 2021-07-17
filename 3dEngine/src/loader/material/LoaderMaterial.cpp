#include <locale>
#include <memory>
#include <UrchinCommon.h>

#include <loader/material/LoaderMaterial.h>
#include <resources/MediaManager.h>
#include <resources/image/Image.h>

namespace urchin {

    std::shared_ptr<Material> LoaderMaterial::loadFromFile(const std::string& filename, const std::map<std::string, std::string>&) {
        std::locale::global(std::locale("C")); //for float

        UdaParser udaParser(FileSystem::instance().getResourcesDirectory() + filename);

        //textures data
        bool repeatableTextures = false;
        auto repeatTexture = udaParser.getUniqueChunk(false, "repeatTexture");
        if (repeatTexture) {
            repeatableTextures = repeatTexture->getBoolValue();
        }

        //diffuse data
        std::shared_ptr<Texture> diffuseTexture;
        auto diffuse = udaParser.getUniqueChunk(false, "diffuse");
        if (diffuse) {
            auto diffuseTextureElem = udaParser.getUniqueChunk(true, "texture", UdaAttribute(), diffuse);
            auto diffuseImage = MediaManager::instance().getMedia<Image>(diffuseTextureElem->getStringValue());
            diffuseTexture = diffuseImage->createTexture(true);
        }

        //normal data
        std::shared_ptr<Texture> normalTexture;
        auto normal = udaParser.getUniqueChunk(false, "normal");
        if (normal) {
            auto normalTextureElem = udaParser.getUniqueChunk(true, "texture", UdaAttribute(), normal);
            auto normalImage = MediaManager::instance().getMedia<Image>(normalTextureElem->getStringValue());
            normalTexture = normalImage->createTexture(true);
        }

        //ambient data
        float fAmbientFactor = 0.0f;
        auto ambient = udaParser.getUniqueChunk(false, "ambient");
        if (ambient) {
            auto ambientFactor = udaParser.getUniqueChunk(true, "factor", UdaAttribute(), ambient);
            fAmbientFactor = TypeConverter::toFloat(ambientFactor->getStringValue());
        }

        return std::make_shared<Material>(diffuseTexture, normalTexture, repeatableTextures, fAmbientFactor);
    }

}
