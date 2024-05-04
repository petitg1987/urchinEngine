#include <random>

#include <graphics/api/GraphicsApi.h>
#include <scene/renderer3d/lighting/shadow/OffsetTextureGenerator.h>

namespace urchin {

    //debug parameters
    bool DEBUG_EXPORT_TEXTURE_DATA = false;

    OffsetTextureGenerator::OffsetTextureGenerator(unsigned int textureXYSize, unsigned int filterXYSize) :
            textureXYSize(textureXYSize),
            filterXYSize(filterXYSize) {
        std::vector<std::vector<std::vector<Vector2<float>>>> textureData = generateTextureData();
        if (DEBUG_EXPORT_TEXTURE_DATA) {
            exportSVG(SystemInfo::homeDirectory() + "shadowMapOffsetTextureData.svg", textureData);
        }

        unsigned int depth = filterXYSize * filterXYSize;
        offsetTexture = Texture::buildArray("SM noise", textureXYSize, textureXYSize, depth, TextureFormat::RG_32_FLOAT, textureData.data(), TextureDataType::FLOAT_32);
    }

    std::shared_ptr<Texture> OffsetTextureGenerator::getOffsetTexture() const {
        return offsetTexture;
    }

    std::vector<std::vector<std::vector<Vector2<float>>>> OffsetTextureGenerator::generateTextureData() const {
        unsigned int seed = 0; //no need to generate different random numbers at each start
        std::uniform_real_distribution<float> randomFloats(-0.5f, 0.5f);
        std::default_random_engine generator(seed);

        std::vector<std::vector<std::vector<Vector2<float>>>> textureData;

        textureData.resize((std::size_t)textureXYSize);
        for (unsigned int texX = 0; texX < textureXYSize; ++texX) {
            textureData[texX].resize((std::size_t)textureXYSize);
            for (unsigned int texY = 0; texY < textureXYSize; ++texY) {
                textureData[texX][texY].resize((std::size_t)filterXYSize * filterXYSize);

                std::size_t filterIndex = 0;
                for (int v = (int)filterXYSize - 1; v >= 0; --v) {
                    for (unsigned int u = 0; u < filterXYSize; ++u) {
                        float pixelRandomX = ((float)u + 0.5f /* move to pixel center */ + randomFloats(generator) /* random inside the pixel */) / (float)filterXYSize;
                        float pixelRandomY = ((float)v + 0.5f /* move to pixel center */ + randomFloats(generator) /* random inside the pixel */) / (float)filterXYSize;

                        //see https://developer.nvidia.com/gpugems/gpugems2/part-ii-shading-lighting-and-shadows/chapter-17-efficient-soft-edged-shadows-using
                        float warpedX = std::sqrt(pixelRandomY) * std::cos(2.0f * MathValue::PI_FLOAT * pixelRandomX);
                        float warpedY = std::sqrt(pixelRandomY) * std::sin(2.0f * MathValue::PI_FLOAT * pixelRandomX);

                        textureData[texX][texY][filterIndex].X = warpedX;
                        textureData[texX][texY][filterIndex].Y = warpedY;
                        filterIndex++;
                    }
                }

            }
        }

        return textureData;
    }

    void OffsetTextureGenerator::exportSVG(std::string filename, const std::vector<std::vector<std::vector<Vector2<float>>>>& textureData) const {
        constexpr float DIST_SEPARATION = 3.0f;

        SVGExporter svgExporter(std::move(filename));
        for (unsigned int texX = 0; texX < textureXYSize; ++texX) {
            float xOffset = (float)texX * DIST_SEPARATION;
            for (unsigned int texY = 0; texY < textureXYSize; ++texY) {
                float yOffset = (float)texY * DIST_SEPARATION;

                std::vector<Point2<float>> pixelPoints;
                pixelPoints.emplace_back(xOffset - 1.0f, yOffset - 1.0f);
                pixelPoints.emplace_back(xOffset + 1.0f, yOffset - 1.0f);
                pixelPoints.emplace_back(xOffset + 1.0f, yOffset + 1.0f);
                pixelPoints.emplace_back(xOffset - 1.0f, yOffset + 1.0f);
                svgExporter.addShape(std::make_unique<SVGPolygon>(pixelPoints, SVGColor::LIME));

                unsigned int filterIndex = 0;
                for (unsigned int filterX = 0; filterX < filterXYSize; ++filterX) {
                    for (unsigned int filterY = 0; filterY < filterXYSize; ++filterY) {
                        SVGColor color = filterIndex < 4 ? SVGColor::RED : SVGColor::BLUE;
                        Point2<float> startVector(xOffset, yOffset);
                        Point2<float> endVector(xOffset + textureData[texX][texY][filterIndex].X, yOffset + textureData[texX][texY][filterIndex].Y);
                        svgExporter.addShape(std::make_unique<SVGLine>(LineSegment2D<float>(startVector, endVector), color, 0.005f));
                        svgExporter.addShape(std::make_unique<SVGCircle>(endVector, 0.02f, color));
                        filterIndex++;
                    }
                }
            }
        }

        svgExporter.generateSVG();
    }

}
