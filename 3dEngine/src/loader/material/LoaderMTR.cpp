#include <locale>
#include <memory>
#include <UrchinCommon.h>

#include <loader/material/LoaderMTR.h>
#include <resources/MediaManager.h>
#include <resources/image/Image.h>

namespace urchin {

    Material* LoaderMTR::loadFromFile(const std::string& filename, const std::map<std::string, std::string>&) {
        std::locale::global(std::locale("C")); //for float

        XmlParser parserXml(filename);

        //textures data
        bool repeatableTextures = false;
        std::shared_ptr<XmlChunk> repeatTexture(parserXml.getUniqueChunk(false, "repeatTexture"));
        if (repeatTexture) {
            repeatableTextures = repeatTexture->getBoolValue();
        }

        //diffuse data
        std::shared_ptr<Texture> diffuseTexture;
        std::shared_ptr<XmlChunk> diffuse(parserXml.getUniqueChunk(false, "diffuse"));
        if (diffuse) {
            std::shared_ptr<XmlChunk> diffuseTextureElem(parserXml.getUniqueChunk(true, "texture", XmlAttribute(), diffuse));
            auto* diffuseImage = MediaManager::instance()->getMedia<Image>(diffuseTextureElem->getStringValue());
            diffuseTexture = diffuseImage->createTexture(true);
            diffuseImage->release();
        }

        //normal data
        std::shared_ptr<Texture> normalTexture;
        std::shared_ptr<XmlChunk> normal(parserXml.getUniqueChunk(false, "normal"));
        if (normal) {
            std::shared_ptr<XmlChunk> normalTextureElem(parserXml.getUniqueChunk(true, "texture", XmlAttribute(), normal));
            auto* normalImage = MediaManager::instance()->getMedia<Image>(normalTextureElem->getStringValue());
            normalTexture = normalImage->createTexture(true);
            normalImage->release();
        }

        //ambient data
        float fAmbientFactor = 0.0;
        std::shared_ptr<XmlChunk> ambient(parserXml.getUniqueChunk(false, "ambient"));
        if (ambient) {
            std::shared_ptr<XmlChunk> ambientFactor(parserXml.getUniqueChunk(true, "factor", XmlAttribute(), ambient));
            fAmbientFactor = TypeConverter::toFloat(ambientFactor->getStringValue());
        }

        return new Material(diffuseTexture, normalTexture, repeatableTextures, fAmbientFactor);
    }

}
