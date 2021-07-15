#include "TerrainReaderWriter.h"

namespace urchin {
    std::unique_ptr<Terrain> TerrainReaderWriter::loadFrom(const UdaChunk* terrainChunk, const UdaParser& udaParser) const {
        std::unique_ptr<Terrain> terrain = buildTerrainFrom(terrainChunk, udaParser);

        loadPropertiesOn(*terrain, terrainChunk, udaParser);
        loadGrassOn(*terrain, terrainChunk, udaParser);

        return terrain;
    }

    void TerrainReaderWriter::writeOn(UdaChunk& terrainChunk, const Terrain& terrain, UdaWriter& udaWriter) const {
        buildChunkFrom(terrainChunk, terrain, udaWriter);

        writePropertiesOn(terrainChunk, terrain, udaWriter);
        writeGrassOn(terrainChunk, terrain, udaWriter);
    }

    std::unique_ptr<Terrain> TerrainReaderWriter::buildTerrainFrom(const UdaChunk* terrainChunk, const UdaParser& udaParser) const {
        auto meshChunk = udaParser.getUniqueChunk(true, MESH_TAG, UdaAttribute(), terrainChunk);
        auto heightFilenameChunk = udaParser.getUniqueChunk(true, HEIGHT_FILENAME_TAG, UdaAttribute(), meshChunk);
        auto xzScaleChunk = udaParser.getUniqueChunk(true, XZ_SCALE_TAG, UdaAttribute(), meshChunk);
        auto yScaleChunk = udaParser.getUniqueChunk(true, Y_SCALE_TAG, UdaAttribute(), meshChunk);
        auto terrainMesh = std::make_unique<TerrainMesh>(heightFilenameChunk->getStringValue(), xzScaleChunk->getFloatValue(), yScaleChunk->getFloatValue());

        auto materialChunk = udaParser.getUniqueChunk(true, MATERIAL_TAG, UdaAttribute(), terrainChunk);
        auto maskMapFilenameChunk = udaParser.getUniqueChunk(true, MASK_MAP_FILENAME, UdaAttribute(), materialChunk);
        auto sRepeatChunk = udaParser.getUniqueChunk(true, S_REPEAT_TAG, UdaAttribute(), materialChunk);
        auto tRepeatChunk = udaParser.getUniqueChunk(true, T_REPEAT_TAG, UdaAttribute(), materialChunk);
        auto materialFilenamesChunk = udaParser.getUniqueChunk(true, MATERIAL_FILENAMES, UdaAttribute(), materialChunk);
        std::vector<std::string> materialFilenames;
        for (unsigned int i = 0; i < TerrainMaterials::MAX_MATERIAL; ++i) {
            auto materialFilenameChunk = udaParser.getUniqueChunk(false, MATERIAL_FILENAME, UdaAttribute(INDEX_ATTR, std::to_string(i)), materialFilenamesChunk);
            if (materialFilenameChunk) {
                materialFilenames.emplace_back(materialFilenameChunk->getStringValue());
            } else {
                materialFilenames.emplace_back("");
            }
        }
        auto terrainMaterial = std::make_unique<TerrainMaterials>(maskMapFilenameChunk->getStringValue(), materialFilenames, sRepeatChunk->getFloatValue(), tRepeatChunk->getFloatValue());

        auto positionChunk = udaParser.getUniqueChunk(true, POSITION_TAG, UdaAttribute(), terrainChunk);

        return std::make_unique<Terrain>(std::move(terrainMesh), std::move(terrainMaterial), positionChunk->getPoint3Value());
    }

    void TerrainReaderWriter::buildChunkFrom(UdaChunk& terrainChunk, const Terrain& terrain, UdaWriter& udaWriter) const {
        auto& meshChunk = udaWriter.createChunk(MESH_TAG, UdaAttribute(), &terrainChunk);
        auto& heightFilenameChunk = udaWriter.createChunk(HEIGHT_FILENAME_TAG, UdaAttribute(), &meshChunk);
        heightFilenameChunk.setStringValue(terrain.getMesh()->getHeightFilename());
        auto& xzScaleChunk = udaWriter.createChunk(XZ_SCALE_TAG, UdaAttribute(), &meshChunk);
        xzScaleChunk.setFloatValue(terrain.getMesh()->getXZScale());
        auto& yScaleChunk = udaWriter.createChunk(Y_SCALE_TAG, UdaAttribute(), &meshChunk);
        yScaleChunk.setFloatValue(terrain.getMesh()->getYScale());

        auto& materialChunk = udaWriter.createChunk(MATERIAL_TAG, UdaAttribute(), &terrainChunk);
        auto& maskMapFilenameChunk = udaWriter.createChunk(MASK_MAP_FILENAME, UdaAttribute(), &materialChunk);
        maskMapFilenameChunk.setStringValue(terrain.getMaterials()->getMaskMapFilename());
        auto& sRepeatChunk = udaWriter.createChunk(S_REPEAT_TAG, UdaAttribute(), &materialChunk);
        sRepeatChunk.setFloatValue(terrain.getMaterials()->getStRepeat().X);
        auto& tRepeatChunk = udaWriter.createChunk(T_REPEAT_TAG, UdaAttribute(), &materialChunk);
        tRepeatChunk.setFloatValue(terrain.getMaterials()->getStRepeat().Y);
        auto& materialFilenamesChunk = udaWriter.createChunk(MATERIAL_FILENAMES, UdaAttribute(), &materialChunk);
        unsigned int i = 0;
        for (const auto& material : terrain.getMaterials()->getMaterials()) {
            if (material != nullptr) {
                auto& materialFilenameChunk = udaWriter.createChunk(MATERIAL_FILENAME, UdaAttribute(INDEX_ATTR, std::to_string(i)), &materialFilenamesChunk);
                materialFilenameChunk.setStringValue(material->getName());
            }
            ++i;
        }

        auto& positionChunk = udaWriter.createChunk(POSITION_TAG, UdaAttribute(), &terrainChunk);
        positionChunk.setPoint3Value(terrain.getPosition());
    }

    void TerrainReaderWriter::loadPropertiesOn(Terrain& terrain, const UdaChunk* terrainChunk, const UdaParser& udaParser) const {
        auto ambientChunk = udaParser.getUniqueChunk(true, AMBIENT_TAG, UdaAttribute(), terrainChunk);
        terrain.setAmbient(ambientChunk->getFloatValue());
    }

    void TerrainReaderWriter::writePropertiesOn(UdaChunk& terrainChunk, const Terrain& terrain, UdaWriter& udaWriter) const {
        auto& ambientChunk = udaWriter.createChunk(AMBIENT_TAG, UdaAttribute(), &terrainChunk);
        ambientChunk.setFloatValue(terrain.getAmbient());
    }

    void TerrainReaderWriter::loadGrassOn(Terrain& terrain, const UdaChunk* terrainChunk, const UdaParser& udaParser) const {
        auto grassChunk = udaParser.getUniqueChunk(false, GRASS_TAG, UdaAttribute(), terrainChunk);
        if (grassChunk) {
            auto grassTextureFilenameChunk = udaParser.getUniqueChunk(true, GRASS_TEXTURE_FILENAME_TAG, UdaAttribute(), grassChunk);
            terrain.getGrass().setGrassTexture(grassTextureFilenameChunk->getStringValue());

            auto grassMaskFilenameChunk = udaParser.getUniqueChunk(true, GRASS_MASK_FILENAME_TAG, UdaAttribute(), grassChunk);
            terrain.getGrass().setMaskTexture(grassMaskFilenameChunk->getStringValue());

            auto numGrassInTexChunk = udaParser.getUniqueChunk(true, NUM_GRASS_IN_TEX_TAG, UdaAttribute(), grassChunk);
            terrain.getGrass().setNumGrassInTexture(numGrassInTexChunk->getUnsignedIntValue());

            auto grassHeightChunk = udaParser.getUniqueChunk(true, GRASS_HEIGHT_TAG, UdaAttribute(), grassChunk);
            terrain.getGrass().setGrassHeight(grassHeightChunk->getFloatValue());

            auto grassLengthChunk = udaParser.getUniqueChunk(true, GRASS_LENGTH_TAG, UdaAttribute(), grassChunk);
            terrain.getGrass().setGrassLength(grassLengthChunk->getFloatValue());

            auto grassQuantityChunk = udaParser.getUniqueChunk(true, GRASS_QUANTITY_TAG, UdaAttribute(), grassChunk);
            terrain.getGrass().setGrassQuantity(grassQuantityChunk->getFloatValue());

            auto windDirectionChunk = udaParser.getUniqueChunk(true, WIND_DIRECTION_TAG, UdaAttribute(), grassChunk);
            terrain.getGrass().setWindDirection(windDirectionChunk->getVector3Value());

            auto windStrengthChunk = udaParser.getUniqueChunk(true, WIND_STRENGTH_TAG, UdaAttribute(), grassChunk);
            terrain.getGrass().setWindStrength(windStrengthChunk->getFloatValue());
        }
    }

    void TerrainReaderWriter::writeGrassOn(UdaChunk& terrainChunk, const Terrain& terrain, UdaWriter& udaWriter) const {
        auto& grassChunk = udaWriter.createChunk(GRASS_TAG, UdaAttribute(), &terrainChunk);

        auto& grassTextureFilenameChunk = udaWriter.createChunk(GRASS_TEXTURE_FILENAME_TAG, UdaAttribute(), &grassChunk);
        grassTextureFilenameChunk.setStringValue(terrain.getGrass().getGrassTexture());

        auto& grassMaskFilenameChunk = udaWriter.createChunk(GRASS_MASK_FILENAME_TAG, UdaAttribute(), &grassChunk);
        grassMaskFilenameChunk.setStringValue(terrain.getGrass().getMaskTexture());

        auto& numGrassInTexChunk = udaWriter.createChunk(NUM_GRASS_IN_TEX_TAG, UdaAttribute(), &grassChunk);
        numGrassInTexChunk.setUnsignedIntValue(terrain.getGrass().getNumGrassInTexture());

        auto& grassHeightChunk = udaWriter.createChunk(GRASS_HEIGHT_TAG, UdaAttribute(), &grassChunk);
        grassHeightChunk.setFloatValue(terrain.getGrass().getGrassHeight());

        auto& grassLengthChunk = udaWriter.createChunk(GRASS_LENGTH_TAG, UdaAttribute(), &grassChunk);
        grassLengthChunk.setFloatValue(terrain.getGrass().getGrassLength());

        auto& grassQuantityChunk = udaWriter.createChunk(GRASS_QUANTITY_TAG, UdaAttribute(), &grassChunk);
        grassQuantityChunk.setFloatValue(terrain.getGrass().getGrassQuantity());

        auto& windDirectionChunk = udaWriter.createChunk(WIND_DIRECTION_TAG, UdaAttribute(), &grassChunk);
        windDirectionChunk.setVector3Value(terrain.getGrass().getWindDirection());

        auto& windStrengthChunk = udaWriter.createChunk(WIND_STRENGTH_TAG, UdaAttribute(), &grassChunk);
        windStrengthChunk.setFloatValue(terrain.getGrass().getWindStrength());
    }
}
