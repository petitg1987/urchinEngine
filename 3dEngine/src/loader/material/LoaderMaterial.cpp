#include <locale>
#include <memory>
#include <UrchinCommon.h>

#include <loader/material/LoaderMaterial.h>
#include <resources/ResourceRetriever.h>
#include <resources/image/Image.h>
#include <resources/material/MaterialBuilder.h>

namespace urchin {

    std::shared_ptr<Material> LoaderMaterial::loadFromFile(const std::string& filename, const std::map<std::string, std::string>&) {
        UdaParser udaParser(FileSystem::instance().getResourcesDirectory() + filename);

        //diffuse texture/color
        bool hasTransparency = false;
        std::shared_ptr<Texture> diffuseTexture;
        auto diffuseChunk = udaParser.getUniqueChunk(true, "diffuse");
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

            Vector4<float> color = diffuseColorChunk->getVector4Value();
            if (color.X > 1.0f || color.Y > 1.0f || color.Z > 1.0f || color.W > 1.0f
                || color.X < 0.0f || color.Y < 0.0f || color.Z < 0.0f || color.W < 0.0f) {
                throw std::runtime_error("Material color must be in range 0.0 - 1.0: " + filename);
            }

            std::vector<unsigned char> rgbaColor({(unsigned char)(255.0f * color.X), (unsigned char)(255.0f * color.Y),
                                                  (unsigned char)(255.0f * color.Z), (unsigned char)(255.0f * color.W)});
            Image diffuseImage(1, 1, Image::IMAGE_RGBA, std::move(rgbaColor), false);
            diffuseTexture = diffuseImage.createTexture(false);
            hasTransparency = !MathFunction::isOne(color.W);
        }
        std::shared_ptr<MaterialBuilder> materialBuilder = MaterialBuilder::create(diffuseTexture, hasTransparency);

        //repeat textures
        auto repeatTexturesChunk = udaParser.getUniqueChunk(false, "repeatTextures");
        if (repeatTexturesChunk && repeatTexturesChunk->getBoolValue()) {
            materialBuilder->enableRepeatTextures();

        }

        //UV scale
        auto uvScaleChunk = udaParser.getUniqueChunk(false, "uvScale");
        if (uvScaleChunk) {
            std::string uScale = udaParser.getUniqueChunk(true, "uScale", UdaAttribute(), uvScaleChunk)->getStringValue();
            std::string vScale = udaParser.getUniqueChunk(true, "vScale", UdaAttribute(), uvScaleChunk)->getStringValue();
            materialBuilder->uvScale(UvScale(toUvScaleType(uScale, filename), toUvScaleType(vScale, filename)));
        }

        //normal texture
        auto normalChunk = udaParser.getUniqueChunk(false, "normal");
        if (normalChunk) {
            auto normalTextureChunk = udaParser.getUniqueChunk(true, "texture", UdaAttribute(), normalChunk);
            auto normalImage = ResourceRetriever::instance().getResource<Image>(normalTextureChunk->getStringValue());
            materialBuilder->normalTexture(normalImage->createTexture(true));
        }

        //emissive factor
        auto emissiveFactorChunk = udaParser.getUniqueChunk(false, "emissiveFactor");
        if (emissiveFactorChunk) {
            materialBuilder->emissiveFactor(emissiveFactorChunk->getFloatValue());
        }

        //ambient factor
        auto ambientFactorChunk = udaParser.getUniqueChunk(false, "ambientFactor");
        if (ambientFactorChunk) {
            materialBuilder->ambientFactor(ambientFactorChunk->getFloatValue());
        }

        //depth test
        auto depthTestChunk = udaParser.getUniqueChunk(false, "depthTest");
        if (depthTestChunk && !depthTestChunk->getBoolValue()) {
            materialBuilder->disableDepthTest();
        }

        //depth write
        auto depthWriteChunk = udaParser.getUniqueChunk(false, "depthWrite");
        if (depthWriteChunk && !depthWriteChunk->getBoolValue()) {
            materialBuilder->disableDepthWrite();
        }

        return materialBuilder->build();
    }

    UvScaleType LoaderMaterial::toUvScaleType(const std::string& uvScaleValue, const std::string& filename) const {
        if (uvScaleValue == UV_SCALE_NONE) {
            return UvScaleType::NONE;
        } else if (uvScaleValue == UV_SCALE_MESH_SCALE_X) {
            return UvScaleType::MESH_SCALE_X;
        } else if (uvScaleValue == UV_SCALE_MESH_SCALE_Y) {
            return UvScaleType::MESH_SCALE_Y;
        } else if (uvScaleValue == UV_SCALE_MESH_SCALE_Z) {
            return UvScaleType::MESH_SCALE_Z;
        }

        throw std::runtime_error("Unknown U/V scale value '" + uvScaleValue + "' for material: " + filename);
    }

}
