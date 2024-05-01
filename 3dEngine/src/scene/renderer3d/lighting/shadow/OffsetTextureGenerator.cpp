#include <random>

#include <scene/renderer3d/lighting/shadow/OffsetTextureGenerator.h>

namespace urchin {

    //debug parameters
    bool DEBUG_EXPORT_TEXTURE_DATA = true; //TODO change to false

    OffsetTextureGenerator::OffsetTextureGenerator(unsigned int textureXYSize, unsigned int filterXYSize) :
            textureXYSize(textureXYSize),
            filterXYSize(filterXYSize) {
        std::vector<Vector2<float>> textureData = generateTextureData();

        if (DEBUG_EXPORT_TEXTURE_DATA) {
            exportSVG(SystemInfo::homeDirectory() + "shadowMapOffsetTextureData.svg", textureData);
        }
    }

    std::vector<Vector2<float>> OffsetTextureGenerator::generateTextureData() const {
        unsigned int seed = 0; //no need to generate different random numbers at each start
        std::uniform_real_distribution<float> randomFloats(-0.5f, 0.5f);
        std::default_random_engine generator(seed);

        std::vector<Vector2<float>> textureData;
        textureData.resize((std::size_t)textureXYSize * (std::size_t)textureXYSize * (std::size_t)filterXYSize * (std::size_t)filterXYSize);

        std::size_t index = 0;
        for (unsigned int texX = 0; texX < textureXYSize; ++texX) {
            for (unsigned int texY = 0; texY < textureXYSize; ++texY) {
                for (unsigned int filterX = 0; filterX < filterXYSize; ++filterX) {
                    for (unsigned int filterY = 0; filterY < filterXYSize; ++filterY) {
                        float pixelRandomX = ((float)filterX + 0.5f /* move to pixel center */ + randomFloats(generator) /* random inside the pixel */) / (float)filterXYSize;
                        float pixelRandomY = ((float)filterY + 0.5f /* move to pixel center */ + randomFloats(generator) /* random inside the pixel */) / (float)filterXYSize;

                        //TODO use warping
                        //float warpedX = std::sqrt(pixelRandomY) * std::cos(2.0f * MathValue::PI_FLOAT * pixelRandomX);
                        //float warpedY = std::sqrt(pixelRandomY) * std::sin(2.0f * MathValue::PI_FLOAT * pixelRandomX);

                        textureData[index].X = pixelRandomX;
                        textureData[index].Y = pixelRandomY;
                        index++;
                    }
                }
            }
        }

        return textureData;
    }

    void OffsetTextureGenerator::exportSVG(std::string filename, const std::vector<Vector2<float>>& textureData) const {
        SVGExporter svgExporter(std::move(filename));

        float separatorDistance = 2.0f;
        float pixelSize = 1.0f / (float)filterXYSize;

        std::size_t index = 0;
        for (unsigned int texX = 0; texX < textureXYSize; ++texX) {
            float xOffset = (float)texX * separatorDistance;
            for (unsigned int texY = 0; texY < textureXYSize; ++texY) {
                float yOffset = (float)texY * separatorDistance;
                for (unsigned int filterX = 0; filterX < filterXYSize; ++filterX) {
                    float xPixelOffset = xOffset + (float)filterX * pixelSize;
                    for (unsigned int filterY = 0; filterY < filterXYSize; ++filterY) {
                        float yPixelOffset = yOffset + (float)filterY * pixelSize;

                        std::vector<Point2<float>> pixelPoints;
                        pixelPoints.emplace_back(xPixelOffset, -yPixelOffset);
                        pixelPoints.emplace_back(xPixelOffset + pixelSize, -yPixelOffset);
                        pixelPoints.emplace_back(xPixelOffset + pixelSize, -(yPixelOffset + pixelSize));
                        pixelPoints.emplace_back(xPixelOffset, -(yPixelOffset + pixelSize));
                        svgExporter.addShape(std::make_unique<SVGPolygon>(pixelPoints, SVGPolygon::LIME));

                        svgExporter.addShape(std::make_unique<SVGCircle>(Point2<float>(xOffset + textureData[index].X, -(yOffset + textureData[index].Y)), 0.01f, SVGPolygon::BLUE));
                        index++;
                    }
                }
            }
        }

        svgExporter.generateSVG();
    }

}
