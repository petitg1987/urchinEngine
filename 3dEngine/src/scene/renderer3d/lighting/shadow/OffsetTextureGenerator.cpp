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

                        //TODO do wrapping

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
        svgExporter.addShape(std::make_unique<SVGCircle>(Point2<float>(0.0f, 0.0f), 1.0f, SVGPolygon::BLUE));
        for (const auto& vectorData : textureData) {
            svgExporter.addShape(std::make_unique<SVGCircle>(Point2<float>(vectorData.X, vectorData.Y), 0.01f, SVGPolygon::LIME));
        }
        svgExporter.generateSVG();
    }

}
