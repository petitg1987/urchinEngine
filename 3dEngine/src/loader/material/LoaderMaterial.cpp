#include <memory>
#include <UrchinCommon.h>

#include <loader/material/LoaderMaterial.h>
#include <resources/ResourceRetriever.h>
#include <resources/material/MaterialBuilder.h>

namespace urchin {

    std::shared_ptr<Material> LoaderMaterial::loadFromFile(const std::string& filename, const std::map<std::string, std::string, std::less<>>&) {
        UdaParser udaParser(filename, UdaLoadType::LOAD_FILE);

        //albedo texture/color
        bool hasTransparency = false;
        std::shared_ptr<Texture> albedoTexture;
        auto albedoChunk = udaParser.getFirstChunk(true, "albedo");
        auto albedoTextureChunk = udaParser.getFirstChunk(false, "texture", UdaAttribute(), albedoChunk);
        if (albedoTextureChunk) {
            albedoTexture = ResourceRetriever::instance().getResource<Texture>(albedoTextureChunk->getStringValue(), {{"mipMap", "1"}});
            hasTransparency = albedoTexture->hasTransparency();
        }

        auto albedoColorChunk = udaParser.getFirstChunk(false, "color", UdaAttribute(), albedoChunk);
        if (albedoColorChunk) {
            if (albedoTexture) {
                throw std::runtime_error("Material defines an albedo while a albedo texture is defined: " + filename);
            }

            Vector4<float> color = albedoColorChunk->getVector4Value();
            if (color.X > 1.0f || color.Y > 1.0f || color.Z > 1.0f || color.W > 1.0f
                || color.X < 0.0f || color.Y < 0.0f || color.Z < 0.0f || color.W < 0.0f) {
                throw std::runtime_error("Material albedo must be in range 0.0 - 1.0: " + filename);
            }

            std::vector<unsigned char> rgbaColor({(unsigned char)(255.0f * color.X), (unsigned char)(255.0f * color.Y),
                                                  (unsigned char)(255.0f * color.Z), (unsigned char)(255.0f * color.W)});
            albedoTexture = Texture::build(filename + " - albedo", 1, 1, TextureFormat::RGBA_8_INT, rgbaColor.data(), TextureDataType::INT_8);
            hasTransparency = !MathFunction::isOne(color.W);
        }
        std::shared_ptr<MaterialBuilder> materialBuilder = MaterialBuilder::create(filename, albedoTexture, hasTransparency);

        //repeat textures
        auto repeatTexturesChunk = udaParser.getFirstChunk(false, "repeatTextures");
        if (repeatTexturesChunk && repeatTexturesChunk->getBoolValue()) {
            materialBuilder->enableRepeatTextures();
        }

        //normal texture
        auto normalChunk = udaParser.getFirstChunk(false, "normal");
        if (normalChunk) {
            auto normalTextureChunk = udaParser.getFirstChunk(true, "texture", UdaAttribute(), normalChunk);
            auto normalTexture = ResourceRetriever::instance().getResource<Texture>(normalTextureChunk->getStringValue(), {{"mipMap", "1"}});
            materialBuilder->normalTexture(normalTexture);
        }

        //roughness texture/value
        auto roughnessChunk = udaParser.getFirstChunk(false, "roughness");
        if (roughnessChunk) {
            std::shared_ptr<Texture> roughnessTexture;

            auto roughnessTextureChunk = udaParser.getFirstChunk(false, "texture", UdaAttribute(), roughnessChunk);
            if (roughnessTextureChunk) {
                roughnessTexture = ResourceRetriever::instance().getResource<Texture>(roughnessTextureChunk->getStringValue(), {{"mipMap", "1"}});
                if (roughnessTexture->getFormat() != TextureFormat::GRAYSCALE_8_INT && roughnessTexture->getFormat() != TextureFormat::GRAYSCALE_16_FLOAT && roughnessTexture->getFormat() != TextureFormat::GRAYSCALE_32_FLOAT) {
                    throw std::runtime_error("Material defines a roughness texture not in grayscale: " + filename);
                }
            }

            auto roughnessValueChunk = udaParser.getFirstChunk(false, "value", UdaAttribute(), roughnessChunk);
            if (roughnessValueChunk) {
                if (roughnessTexture) {
                    throw std::runtime_error("Material defines a roughness value while a roughness texture is defined: " + filename);
                }

                float roughness = roughnessValueChunk->getFloatValue();
                if (roughness > 1.0f || roughness < 0.05f) {
                    throw std::runtime_error("Material roughness must be in range 0.05 - 1.0: " + filename);
                }

                std::vector<unsigned char> roughnessTextureData({(unsigned char) (255.0f * roughness)});
                roughnessTexture = Texture::build(filename + " - roughness", 1, 1, TextureFormat::GRAYSCALE_8_INT, roughnessTextureData.data(), TextureDataType::INT_8);
            }

            if (roughnessTexture) {
                materialBuilder->roughnessTexture(roughnessTexture);
            }
        }

        //metalness texture/value
        auto metalnessChunk = udaParser.getFirstChunk(false, "metalness");
        if (metalnessChunk) {
            std::shared_ptr<Texture> metalnessTexture;

            auto metalnessTextureChunk = udaParser.getFirstChunk(false, "texture", UdaAttribute(), metalnessChunk);
            if (metalnessTextureChunk) {
                metalnessTexture = ResourceRetriever::instance().getResource<Texture>(metalnessTextureChunk->getStringValue(), {{"mipMap", "1"}});
                if (metalnessTexture->getFormat() != TextureFormat::GRAYSCALE_8_INT && metalnessTexture->getFormat() != TextureFormat::GRAYSCALE_16_FLOAT && metalnessTexture->getFormat() != TextureFormat::GRAYSCALE_32_FLOAT) {
                    throw std::runtime_error("Material defines a metalness texture not in grayscale: " + filename);
                }
            }

            auto metalnessValueChunk = udaParser.getFirstChunk(false, "value", UdaAttribute(), metalnessChunk);
            if (metalnessValueChunk) {
                if (metalnessTexture) {
                    throw std::runtime_error("Material defines a metalness value while a metalness texture is defined: " + filename);
                }

                float metalness = metalnessValueChunk->getFloatValue();
                if (metalness > 1.0f || metalness < 0.0f) {
                    throw std::runtime_error("Material metalness must be in range 0.0 - 1.0: " + filename);
                }

                std::vector<unsigned char> metalnessTextureData({(unsigned char) (255.0f * metalness)});
                metalnessTexture = Texture::build(filename + " - metalness", 1, 1, TextureFormat::GRAYSCALE_8_INT, metalnessTextureData.data(), TextureDataType::INT_8);
            }

            if (metalnessTexture) {
                materialBuilder->metalnessTexture(metalnessTexture);
            }
        }

        //UV scale
        auto uvScaleChunk = udaParser.getFirstChunk(false, "uvScale");
        if (uvScaleChunk) {
            std::string scaleTypeValue = udaParser.getFirstChunk(true, "scaleType", UdaAttribute(), uvScaleChunk)->getStringValue();
            UvScaleType uvScaleType = toUvScaleType(scaleTypeValue, filename);

            MeshScaleAxis uMeshScaleAxis = MeshScaleAxis::NONE;
            MeshScaleAxis vMeshScaleAxis = MeshScaleAxis::NONE;
            if (uvScaleType == UvScaleType::SCALE_ON_MESH_SCALE) {
                std::string uMeshScaleAxisValue = udaParser.getFirstChunk(true, "uMeshScaleAxis", UdaAttribute(), uvScaleChunk)->getStringValue();
                uMeshScaleAxis = toMeshScaleAxis(uMeshScaleAxisValue, filename);
                std::string vScaleMeshValue = udaParser.getFirstChunk(true, "vMeshScaleAxis", UdaAttribute(), uvScaleChunk)->getStringValue();
                vMeshScaleAxis = toMeshScaleAxis(vScaleMeshValue, filename);
            }

            materialBuilder->uvScale(UvScale(uvScaleType, uMeshScaleAxis, vMeshScaleAxis));
        }

        //emissive factor
        auto emissiveFactorChunk = udaParser.getFirstChunk(false, "emissiveFactor");
        if (emissiveFactorChunk) {
            materialBuilder->emissiveFactor(emissiveFactorChunk->getFloatValue());
        }

        //ambient factor
        auto ambientFactorChunk = udaParser.getFirstChunk(false, "ambientFactor");
        if (ambientFactorChunk) {
            materialBuilder->ambientFactor(ambientFactorChunk->getFloatValue());
        }

        //depth test
        auto depthTestChunk = udaParser.getFirstChunk(false, "depthTest");
        if (depthTestChunk && !depthTestChunk->getBoolValue()) {
            materialBuilder->disableDepthTest();
        }

        //depth write
        auto depthWriteChunk = udaParser.getFirstChunk(false, "depthWrite");
        if (depthWriteChunk && !depthWriteChunk->getBoolValue()) {
            materialBuilder->disableDepthWrite();
        }

        return materialBuilder->build();
    }

    UvScaleType LoaderMaterial::toUvScaleType(std::string_view scaleTypeValue, const std::string& filename) const {
        if (scaleTypeValue == UV_SCALE_NONE) {
            return UvScaleType::NONE;
        } else if (scaleTypeValue == SCALE_ON_AXIS_ALIGNED_FACES) {
            return UvScaleType::SCALE_ON_AXIS_ALIGNED_FACES;
        } else if (scaleTypeValue == SCALE_ON_XY_FACES) {
            return UvScaleType::SCALE_ON_XY_FACES;
        } else if (scaleTypeValue == SCALE_ON_XZ_FACES) {
            return UvScaleType::SCALE_ON_XZ_FACES;
        } else if (scaleTypeValue == SCALE_ON_YZ_FACES) {
            return UvScaleType::SCALE_ON_YZ_FACES;
        } else if (scaleTypeValue == SCALE_ON_MESH_SCALE) {
            return UvScaleType::SCALE_ON_MESH_SCALE;
        }

        throw std::runtime_error("Unknown UV scale value '" + std::string(scaleTypeValue) + "' for material: " + filename);
    }

    MeshScaleAxis LoaderMaterial::toMeshScaleAxis(std::string_view meshScaleAxisValue, const std::string& filename) const {
         if (meshScaleAxisValue == MESH_SCALE_AXIS_X) {
            return MeshScaleAxis::X;
        } else if (meshScaleAxisValue == MESH_SCALE_AXIS_Y) {
            return MeshScaleAxis::Y;
        } else if (meshScaleAxisValue == MESH_SCALE_AXIS_Z) {
            return MeshScaleAxis::Z;
        } else if (meshScaleAxisValue == MESH_SCALE_AXIS_NONE) {
            return MeshScaleAxis::NONE;
        }

        throw std::runtime_error("Unknown mesh scale axis value '" + std::string(meshScaleAxisValue) + "' for material: " + filename);
    }

}
