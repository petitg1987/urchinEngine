#include "TerrainReaderWriter.h"

namespace urchin {
    Terrain* TerrainReaderWriter::loadFrom(const DataChunk* terrainChunk, const DataParser& dataParser) const {
        Terrain* terrain = buildTerrainFrom(terrainChunk, dataParser);

        loadPropertiesOn(terrain, terrainChunk, dataParser);
        loadGrassOn(terrain, terrainChunk, dataParser);

        return terrain;
    }

    void TerrainReaderWriter::writeOn(DataChunk& terrainChunk, const Terrain* terrain, DataWriter& dataWriter) const {
        buildChunkFrom(terrainChunk, terrain, dataWriter);

        writePropertiesOn(terrainChunk, terrain, dataWriter);
        writeGrassOn(terrainChunk, terrain, dataWriter);
    }

    Terrain* TerrainReaderWriter::buildTerrainFrom(const DataChunk* terrainChunk, const DataParser& dataParser) const {
        auto meshChunk = dataParser.getUniqueChunk(true, MESH_TAG, DataAttribute(), terrainChunk);
        auto heightFilenameChunk = dataParser.getUniqueChunk(true, HEIGHT_FILENAME_TAG, DataAttribute(), meshChunk);
        auto xzScaleChunk = dataParser.getUniqueChunk(true, XZ_SCALE_TAG, DataAttribute(), meshChunk);
        auto yScaleChunk = dataParser.getUniqueChunk(true, Y_SCALE_TAG, DataAttribute(), meshChunk);
        auto terrainMesh = std::make_unique<TerrainMesh>(heightFilenameChunk->getStringValue(), xzScaleChunk->getFloatValue(), yScaleChunk->getFloatValue());

        auto materialChunk = dataParser.getUniqueChunk(true, MATERIAL_TAG, DataAttribute(), terrainChunk);
        auto maskMapFilenameChunk = dataParser.getUniqueChunk(true, MASK_MAP_FILENAME, DataAttribute(), materialChunk);
        auto sRepeatChunk = dataParser.getUniqueChunk(true, S_REPEAT_TAG, DataAttribute(), materialChunk);
        auto tRepeatChunk = dataParser.getUniqueChunk(true, T_REPEAT_TAG, DataAttribute(), materialChunk);
        auto materialFilenamesChunk = dataParser.getUniqueChunk(true, MATERIAL_FILENAMES, DataAttribute(), materialChunk);
        std::vector<std::string> materialFilenames;
        for (unsigned int i = 0; i < TerrainMaterials::MAX_MATERIAL; ++i) {
            auto materialFilenameChunk = dataParser.getUniqueChunk(false, MATERIAL_FILENAME, DataAttribute(INDEX_ATTR, std::to_string(i)), materialFilenamesChunk);
            if (materialFilenameChunk) {
                materialFilenames.emplace_back(materialFilenameChunk->getStringValue());
            } else {
                materialFilenames.emplace_back("");
            }
        }
        auto terrainMaterial = std::make_unique<TerrainMaterials>(maskMapFilenameChunk->getStringValue(), materialFilenames, sRepeatChunk->getFloatValue(), tRepeatChunk->getFloatValue());

        auto positionChunk = dataParser.getUniqueChunk(true, POSITION_TAG, DataAttribute(), terrainChunk);

        return new Terrain(std::move(terrainMesh), std::move(terrainMaterial), positionChunk->getPoint3Value());
    }

    void TerrainReaderWriter::buildChunkFrom(DataChunk& terrainChunk, const Terrain* terrain, DataWriter& dataWriter) const {
        auto& meshChunk = dataWriter.createChunk(MESH_TAG, DataAttribute(), &terrainChunk);
        auto& heightFilenameChunk = dataWriter.createChunk(HEIGHT_FILENAME_TAG, DataAttribute(), &meshChunk);
        heightFilenameChunk.setStringValue(terrain->getMesh()->getHeightFilename());
        auto& xzScaleChunk = dataWriter.createChunk(XZ_SCALE_TAG, DataAttribute(), &meshChunk);
        xzScaleChunk.setFloatValue(terrain->getMesh()->getXZScale());
        auto& yScaleChunk = dataWriter.createChunk(Y_SCALE_TAG, DataAttribute(), &meshChunk);
        yScaleChunk.setFloatValue(terrain->getMesh()->getYScale());

        auto& materialChunk = dataWriter.createChunk(MATERIAL_TAG, DataAttribute(), &terrainChunk);
        auto& maskMapFilenameChunk = dataWriter.createChunk(MASK_MAP_FILENAME, DataAttribute(), &materialChunk);
        maskMapFilenameChunk.setStringValue(terrain->getMaterials()->getMaskMapFilename());
        auto& sRepeatChunk = dataWriter.createChunk(S_REPEAT_TAG, DataAttribute(), &materialChunk);
        sRepeatChunk.setFloatValue(terrain->getMaterials()->getStRepeat().X);
        auto& tRepeatChunk = dataWriter.createChunk(T_REPEAT_TAG, DataAttribute(), &materialChunk);
        tRepeatChunk.setFloatValue(terrain->getMaterials()->getStRepeat().Y);
        auto& materialFilenamesChunk = dataWriter.createChunk(MATERIAL_FILENAMES, DataAttribute(), &materialChunk);
        unsigned int i = 0;
        for (const Material* material : terrain->getMaterials()->getMaterials()) {
            if (material != nullptr) {
                auto& materialFilenameChunk = dataWriter.createChunk(MATERIAL_FILENAME, DataAttribute(INDEX_ATTR, std::to_string(i)), &materialFilenamesChunk);
                materialFilenameChunk.setStringValue(material->getName());
            }
            ++i;
        }

        auto& positionChunk = dataWriter.createChunk(POSITION_TAG, DataAttribute(), &terrainChunk);
        positionChunk.setPoint3Value(terrain->getPosition());
    }

    void TerrainReaderWriter::loadPropertiesOn(Terrain* terrain, const DataChunk* terrainChunk, const DataParser& dataParser) const {
        auto ambientChunk = dataParser.getUniqueChunk(true, AMBIENT_TAG, DataAttribute(), terrainChunk);
        terrain->setAmbient(ambientChunk->getFloatValue());
    }

    void TerrainReaderWriter::writePropertiesOn(DataChunk& terrainChunk, const Terrain* terrain, DataWriter& dataWriter) const {
        auto& ambientChunk = dataWriter.createChunk(AMBIENT_TAG, DataAttribute(), &terrainChunk);
        ambientChunk.setFloatValue(terrain->getAmbient());
    }

    void TerrainReaderWriter::loadGrassOn(Terrain* terrain, const DataChunk* terrainChunk, const DataParser& dataParser) const {
        auto grassChunk = dataParser.getUniqueChunk(false, GRASS_TAG, DataAttribute(), terrainChunk);
        if (grassChunk) {
            auto grassTextureFilenameChunk = dataParser.getUniqueChunk(true, GRASS_TEXTURE_FILENAME_TAG, DataAttribute(), grassChunk);
            terrain->getGrass()->setGrassTexture(grassTextureFilenameChunk->getStringValue());

            auto grassMaskFilenameChunk = dataParser.getUniqueChunk(true, GRASS_MASK_FILENAME_TAG, DataAttribute(), grassChunk);
            terrain->getGrass()->setMaskTexture(grassMaskFilenameChunk->getStringValue());

            auto numGrassInTexChunk = dataParser.getUniqueChunk(true, NUM_GRASS_IN_TEX_TAG, DataAttribute(), grassChunk);
            terrain->getGrass()->setNumGrassInTexture(numGrassInTexChunk->getUnsignedIntValue());

            auto grassHeightChunk = dataParser.getUniqueChunk(true, GRASS_HEIGHT_TAG, DataAttribute(), grassChunk);
            terrain->getGrass()->setGrassHeight(grassHeightChunk->getFloatValue());

            auto grassLengthChunk = dataParser.getUniqueChunk(true, GRASS_LENGTH_TAG, DataAttribute(), grassChunk);
            terrain->getGrass()->setGrassLength(grassLengthChunk->getFloatValue());

            auto grassQuantityChunk = dataParser.getUniqueChunk(true, GRASS_QUANTITY_TAG, DataAttribute(), grassChunk);
            terrain->getGrass()->setGrassQuantity(grassQuantityChunk->getFloatValue());

            auto windDirectionChunk = dataParser.getUniqueChunk(true, WIND_DIRECTION_TAG, DataAttribute(), grassChunk);
            terrain->getGrass()->setWindDirection(windDirectionChunk->getVector3Value());

            auto windStrengthChunk = dataParser.getUniqueChunk(true, WIND_STRENGTH_TAG, DataAttribute(), grassChunk);
            terrain->getGrass()->setWindStrength(windStrengthChunk->getFloatValue());
        }
    }

    void TerrainReaderWriter::writeGrassOn(DataChunk& terrainChunk, const Terrain* terrain, DataWriter& dataWriter) const {
        if (terrain->getGrass()) {
            auto& grassChunk = dataWriter.createChunk(GRASS_TAG, DataAttribute(), &terrainChunk);

            auto& grassTextureFilenameChunk = dataWriter.createChunk(GRASS_TEXTURE_FILENAME_TAG, DataAttribute(), &grassChunk);
            grassTextureFilenameChunk.setStringValue(terrain->getGrass()->getGrassTexture());

            auto& grassMaskFilenameChunk = dataWriter.createChunk(GRASS_MASK_FILENAME_TAG, DataAttribute(), &grassChunk);
            grassMaskFilenameChunk.setStringValue(terrain->getGrass()->getMaskTexture());

            auto& numGrassInTexChunk = dataWriter.createChunk(NUM_GRASS_IN_TEX_TAG, DataAttribute(), &grassChunk);
            numGrassInTexChunk.setUnsignedIntValue(terrain->getGrass()->getNumGrassInTexture());

            auto& grassHeightChunk = dataWriter.createChunk(GRASS_HEIGHT_TAG, DataAttribute(), &grassChunk);
            grassHeightChunk.setFloatValue(terrain->getGrass()->getGrassHeight());

            auto& grassLengthChunk = dataWriter.createChunk(GRASS_LENGTH_TAG, DataAttribute(), &grassChunk);
            grassLengthChunk.setFloatValue(terrain->getGrass()->getGrassLength());

            auto& grassQuantityChunk = dataWriter.createChunk(GRASS_QUANTITY_TAG, DataAttribute(), &grassChunk);
            grassQuantityChunk.setFloatValue(terrain->getGrass()->getGrassQuantity());

            auto& windDirectionChunk = dataWriter.createChunk(WIND_DIRECTION_TAG, DataAttribute(), &grassChunk);
            windDirectionChunk.setVector3Value(terrain->getGrass()->getWindDirection());

            auto& windStrengthChunk = dataWriter.createChunk(WIND_STRENGTH_TAG, DataAttribute(), &grassChunk);
            windStrengthChunk.setFloatValue(terrain->getGrass()->getWindStrength());
        }
    }
}
