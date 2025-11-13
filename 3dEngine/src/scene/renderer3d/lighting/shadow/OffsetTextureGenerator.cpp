#include <random>

#include "scene/renderer3d/lighting/shadow/OffsetTextureGenerator.h"

namespace urchin {

    //debug parameters
    constexpr auto DEBUG_EXPORT_TEXTURE_DATA = False();

    OffsetTextureGenerator::OffsetTextureGenerator(unsigned int textureXYSize, unsigned int filterBoxSize) :
            textureXYSize(textureXYSize),
            filterBoxSize(filterBoxSize) {

        #ifdef URCHIN_DEBUG
            assert(textureXYSize > 0);
            assert(filterBoxSize > 0);
        #endif

        if (filterBoxSize == 1) {
            std::vector textureData = {Vector2(0.0f, 0.0f)};
            offsetTexture = Texture::buildArray("SM offset", 1, 1, 1, TextureFormat::RG_32_FLOAT, textureData.data(), false, TextureDataType::FLOAT_32);
        } else {
            std::vector<Vector2<float>> textureData = generateTextureData();
            if (DEBUG_EXPORT_TEXTURE_DATA) {
                exportSVG(SystemInfo::homeDirectory() + "shadowMapOffsetTextureData.svg", textureData);
            }

            unsigned int filterSize = filterBoxSize * filterBoxSize;
            offsetTexture = Texture::buildArray("SM offset", textureXYSize, textureXYSize, filterSize, TextureFormat::RG_32_FLOAT, textureData.data(), false, TextureDataType::FLOAT_32);
        }
    }

    std::shared_ptr<Texture> OffsetTextureGenerator::getOffsetTexture() const {
        return offsetTexture;
    }

    std::vector<Vector2<float>> OffsetTextureGenerator::generateTextureData() const {
        std::size_t numVector = (std::size_t)textureXYSize * textureXYSize * filterBoxSize * filterBoxSize;

        unsigned int seed = 0; //no need to generate different random numbers at each start
        std::uniform_real_distribution randomFloats(-0.5f, 0.5f);
        std::default_random_engine generator(seed);

        std::vector<Vector2<float>> textureData;
        textureData.resize(numVector);

        std::size_t index = 0;
        for (int v = (int)filterBoxSize - 1; v >= 0; --v) {
            for (unsigned int u = 0; u < filterBoxSize; ++u) {
                for (unsigned int texX = 0; texX < textureXYSize; ++texX) {
                    for (unsigned int texY = 0; texY < textureXYSize; ++texY) {
                        float pixelRandomX = ((float)u + 0.5f /* move to pixel center */ + randomFloats(generator) /* random inside the pixel */) / (float)filterBoxSize;
                        float pixelRandomY = ((float)v + 0.5f /* move to pixel center */ + randomFloats(generator) /* random inside the pixel */) / (float)filterBoxSize;

                        //see https://developer.nvidia.com/gpugems/gpugems2/part-ii-shading-lighting-and-shadows/chapter-17-efficient-soft-edged-shadows-using
                        float warpedX = std::sqrt(pixelRandomY) * std::cos(2.0f * MathValue::PI_FLOAT * pixelRandomX);
                        float warpedY = std::sqrt(pixelRandomY) * std::sin(2.0f * MathValue::PI_FLOAT * pixelRandomX);

                        textureData[index].X = warpedX;
                        textureData[index].Y = warpedY;

                        index++;
                    }
                }
            }
        }

        return textureData;
    }

    void OffsetTextureGenerator::exportSVG(std::string filename, const std::vector<Vector2<float>>& textureData) const {
        constexpr float DIST_SEPARATION = 3.0f;

        SVGExporter svgExporter(std::move(filename));
        unsigned int filterIndex = 0;
        for (unsigned int filterX = 0; filterX < filterBoxSize; ++filterX) {
            for (unsigned int filterY = 0; filterY < filterBoxSize; ++filterY) {

                for (unsigned int texX = 0; texX < textureXYSize; ++texX) {
                    float xOffset = (float)texX * DIST_SEPARATION;
                    for (unsigned int texY = 0; texY < textureXYSize; ++texY) {
                        float yOffset = (float)texY * DIST_SEPARATION;

                        std::size_t index = (filterIndex * textureXYSize * textureXYSize) + (texX * textureXYSize) + texY;

                        SVGColor color = filterIndex < 4 ? RED : BLUE;
                        Point2 startVector(xOffset, yOffset);
                        Point2 endVector(xOffset + textureData[index].X, yOffset + textureData[index].Y);
                        svgExporter.addShape(std::make_unique<SVGLine>(LineSegment2D(startVector, endVector), color, 0.005f));
                        svgExporter.addShape(std::make_unique<SVGCircle>(endVector, 0.02f, color));
                    }
                }

                filterIndex++;
            }
        }

        svgExporter.generateSVG();
    }

}
