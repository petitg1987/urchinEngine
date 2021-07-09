#include <locale>
#include <memory>
#include <UrchinCommon.h>

#include <loader/material/LoaderMaterial.h>
#include <resources/MediaManager.h>
#include <resources/image/Image.h>

namespace urchin {

    Material* LoaderMaterial::loadFromFile(const std::string& filename, const std::map<std::string, std::string>&) {
        std::locale::global(std::locale("C")); //for float

        DataParser dataParser(filename);

        //textures data
        bool repeatableTextures = false;
        auto repeatTexture = dataParser.getUniqueChunk(false, "repeatTexture");
        if (repeatTexture) {
            repeatableTextures = repeatTexture->getBoolValue();
        }

        //diffuse data
        std::shared_ptr<Texture> diffuseTexture;
        auto diffuse = dataParser.getUniqueChunk(false, "diffuse");
        if (diffuse) {
            auto diffuseTextureElem = dataParser.getUniqueChunk(true, "texture", DataAttribute(), diffuse.get());
            auto* diffuseImage = MediaManager::instance()->getMedia<Image>(diffuseTextureElem->getStringValue());
            diffuseTexture = diffuseImage->createTexture(true);
            diffuseImage->release();
        }

        //normal data
        std::shared_ptr<Texture> normalTexture;
        auto normal = dataParser.getUniqueChunk(false, "normal");
        if (normal) {
            auto normalTextureElem = dataParser.getUniqueChunk(true, "texture", DataAttribute(), normal.get());
            auto* normalImage = MediaManager::instance()->getMedia<Image>(normalTextureElem->getStringValue());
            normalTexture = normalImage->createTexture(true);
            normalImage->release();
        }

        //ambient data
        float fAmbientFactor = 0.0;
        auto ambient = dataParser.getUniqueChunk(false, "ambient");
        if (ambient) {
            auto ambientFactor = dataParser.getUniqueChunk(true, "factor", DataAttribute(), ambient.get());
            fAmbientFactor = TypeConverter::toFloat(ambientFactor->getStringValue());
        }

        return new Material(diffuseTexture, normalTexture, repeatableTextures, fAmbientFactor);
    }

}
