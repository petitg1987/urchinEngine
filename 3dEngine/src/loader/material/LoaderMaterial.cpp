#include <locale>
#include <memory>
#include <UrchinCommon.h>

#include <loader/material/LoaderMaterial.h>
#include <resources/ResourceRetriever.h>
#include <resources/image/Image.h>

namespace urchin {

    std::shared_ptr<Material> LoaderMaterial::loadFromFile(const std::string& filename, const std::map<std::string, std::string>&) {
        UdaParser udaParser(FileSystem::instance().getResourcesDirectory() + filename);

        //textures data
        bool repeatTextures = false;
        auto repeatTexturesChunk = udaParser.getUniqueChunk(false, "repeatTextures");
        if (repeatTexturesChunk) {
            repeatTextures = repeatTexturesChunk->getBoolValue();
        }

        //diffuse texture/color
        bool hasTransparency = false;
        std::shared_ptr<Texture> diffuseTexture;
        auto diffuseChunk = udaParser.getUniqueChunk(false, "diffuse");
        if (diffuseChunk) {
            auto diffuseTextureChunk = udaParser.getUniqueChunk(false, "texture", UdaAttribute(), diffuseChunk);
            if (diffuseTextureChunk) {
                auto diffuseImage = ResourceRetriever::instance().getResource<Image>(diffuseTextureChunk->getStringValue());
                diffuseTexture = diffuseImage->createTexture(true);
                hasTransparency = diffuseImage->hasTransparency();
            }

            auto diffuseColorChunk = udaParser.getUniqueChunk(false, "color", UdaAttribute(), diffuseChunk);
            if (diffuseColorChunk) {
                if (diffuseTexture) {
                    throw std::runtime_error("Material defines a diffuse color while a diffuse texture is defined: " + filename);
                }

                Vector3<float> color = diffuseColorChunk->getVector3Value();
                if (color.X > 1.0f || color.Y > 1.0f || color.Z > 1.0f || color.X < 0.0f || color.Y < 0.0f || color.Z < 0.0f) {
                    throw std::runtime_error("Material color must be in range 0.0 - 1.0: " + filename);
                }

                std::vector<unsigned char> rgbaColor({(unsigned char)(255.0f * color.X), (unsigned char)(255.0f * color.Y),
                                                      (unsigned char)(255.0f * color.Z), 255});
                Image diffuseImage(1, 1, Image::IMAGE_RGBA, std::move(rgbaColor), false);
                diffuseTexture = diffuseImage.createTexture(false);
            }
        }

        //normal texture
        std::shared_ptr<Texture> normalTexture;
        auto normalChunk = udaParser.getUniqueChunk(false, "normal");
        if (normalChunk) {
            auto normalTextureChunk = udaParser.getUniqueChunk(true, "texture", UdaAttribute(), normalChunk);
            auto normalImage = ResourceRetriever::instance().getResource<Image>(normalTextureChunk->getStringValue());
            normalTexture = normalImage->createTexture(true);
        }

        //emissive factor
        float emissiveFactor = 1.0f;
        auto emissiveFactorChunk = udaParser.getUniqueChunk(false, "emissiveFactor");
        if (emissiveFactorChunk) {
            emissiveFactor = emissiveFactorChunk->getFloatValue();
        }

        //ambient factor
        float ambientFactor = 0.0f;
        auto ambientFactorChunk = udaParser.getUniqueChunk(false, "ambientFactor");
        if (ambientFactorChunk) {
            ambientFactor = ambientFactorChunk->getFloatValue();
        }

        return std::make_shared<Material>(repeatTextures, diffuseTexture, normalTexture, hasTransparency, emissiveFactor, ambientFactor);
    }

}
