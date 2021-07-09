#include "TerrainReaderWriter.h"

namespace urchin {
    Terrain* TerrainReaderWriter::loadFrom(const XmlChunk* terrainChunk, const XmlParser& xmlParser) const {
        Terrain* terrain = buildTerrainFrom(terrainChunk, xmlParser);

        loadPropertiesOn(terrain, terrainChunk, xmlParser);
        loadGrassOn(terrain, terrainChunk, xmlParser);

        return terrain;
    }

    void TerrainReaderWriter::writeOn(XmlChunk* terrainChunk, const Terrain* terrain, XmlWriter& xmlWriter) const {
        buildChunkFrom(terrainChunk, terrain, xmlWriter);

        writePropertiesOn(terrainChunk, terrain, xmlWriter);
        writeGrassOn(terrainChunk, terrain, xmlWriter);
    }

    Terrain* TerrainReaderWriter::buildTerrainFrom(const XmlChunk* terrainChunk, const XmlParser& xmlParser) const {
        auto meshChunk = xmlParser.getUniqueChunk(true, MESH_TAG, XmlAttribute(), terrainChunk);
        auto heightFilenameChunk = xmlParser.getUniqueChunk(true, HEIGHT_FILENAME_TAG, XmlAttribute(), meshChunk.get());
        auto xzScaleChunk = xmlParser.getUniqueChunk(true, XZ_SCALE_TAG, XmlAttribute(), meshChunk.get());
        auto yScaleChunk = xmlParser.getUniqueChunk(true, Y_SCALE_TAG, XmlAttribute(), meshChunk.get());
        auto terrainMesh = std::make_shared<TerrainMesh>(heightFilenameChunk->getStringValue(), xzScaleChunk->getFloatValue(), yScaleChunk->getFloatValue());

        auto materialChunk = xmlParser.getUniqueChunk(true, MATERIAL_TAG, XmlAttribute(), terrainChunk);
        auto maskMapFilenameChunk = xmlParser.getUniqueChunk(true, MASK_MAP_FILENAME, XmlAttribute(), materialChunk.get());
        auto sRepeatChunk = xmlParser.getUniqueChunk(true, S_REPEAT_TAG, XmlAttribute(), materialChunk.get());
        auto tRepeatChunk = xmlParser.getUniqueChunk(true, T_REPEAT_TAG, XmlAttribute(), materialChunk.get());
        auto materialFilenamesChunk = xmlParser.getUniqueChunk(true, MATERIAL_FILENAMES, XmlAttribute(), materialChunk.get());
        std::vector<std::string> materialFilenames;
        for (unsigned int i = 0; i < TerrainMaterials::MAX_MATERIAL; ++i) {
            auto materialFilenameChunk = xmlParser.getUniqueChunk(false, MATERIAL_FILENAME, XmlAttribute(INDEX_ATTR, std::to_string(i)), materialFilenamesChunk.get());
            if (materialFilenameChunk) {
                materialFilenames.emplace_back(materialFilenameChunk->getStringValue());
            } else {
                materialFilenames.emplace_back("");
            }
        }
        auto terrainMaterial = std::make_unique<TerrainMaterials>(maskMapFilenameChunk->getStringValue(), materialFilenames, sRepeatChunk->getFloatValue(), tRepeatChunk->getFloatValue());

        auto positionChunk = xmlParser.getUniqueChunk(true, POSITION_TAG, XmlAttribute(), terrainChunk);

        return new Terrain(terrainMesh, std::move(terrainMaterial), positionChunk->getPoint3Value());
    }

    void TerrainReaderWriter::buildChunkFrom(const XmlChunk* terrainChunk, const Terrain* terrain, XmlWriter& xmlWriter) const {
        auto meshChunk = xmlWriter.createChunk(MESH_TAG, XmlAttribute(), terrainChunk);
        auto heightFilenameChunk = xmlWriter.createChunk(HEIGHT_FILENAME_TAG, XmlAttribute(), meshChunk.get());
        heightFilenameChunk->setStringValue(terrain->getMesh()->getHeightFilename());
        auto xzScaleChunk = xmlWriter.createChunk(XZ_SCALE_TAG, XmlAttribute(), meshChunk.get());
        xzScaleChunk->setFloatValue(terrain->getMesh()->getXZScale());
        auto yScaleChunk = xmlWriter.createChunk(Y_SCALE_TAG, XmlAttribute(), meshChunk.get());
        yScaleChunk->setFloatValue(terrain->getMesh()->getYScale());

        auto materialChunk = xmlWriter.createChunk(MATERIAL_TAG, XmlAttribute(), terrainChunk);
        auto maskMapFilenameChunk = xmlWriter.createChunk(MASK_MAP_FILENAME, XmlAttribute(), materialChunk.get());
        maskMapFilenameChunk->setStringValue(terrain->getMaterials()->getMaskMapFilename());
        auto sRepeatChunk = xmlWriter.createChunk(S_REPEAT_TAG, XmlAttribute(), materialChunk.get());
        sRepeatChunk->setFloatValue(terrain->getMaterials()->getStRepeat().X);
        auto tRepeatChunk = xmlWriter.createChunk(T_REPEAT_TAG, XmlAttribute(), materialChunk.get());
        tRepeatChunk->setFloatValue(terrain->getMaterials()->getStRepeat().Y);
        auto materialFilenamesChunk = xmlWriter.createChunk(MATERIAL_FILENAMES, XmlAttribute(), materialChunk.get());
        unsigned int i = 0;
        for (const Material* material : terrain->getMaterials()->getMaterials()) {
            if (material != nullptr) {
                auto materialFilenameChunk = xmlWriter.createChunk(MATERIAL_FILENAME, XmlAttribute(INDEX_ATTR, std::to_string(i)), materialFilenamesChunk.get());
                materialFilenameChunk->setStringValue(material->getName());
            }
            ++i;
        }

        auto positionChunk = xmlWriter.createChunk(POSITION_TAG, XmlAttribute(), terrainChunk);
        positionChunk->setPoint3Value(terrain->getPosition());
    }

    void TerrainReaderWriter::loadPropertiesOn(Terrain* terrain, const XmlChunk* terrainChunk, const XmlParser& xmlParser) const {
        auto ambientChunk = xmlParser.getUniqueChunk(true, AMBIENT_TAG, XmlAttribute(), terrainChunk);
        terrain->setAmbient(ambientChunk->getFloatValue());
    }

    void TerrainReaderWriter::writePropertiesOn(const XmlChunk* terrainChunk, const Terrain* terrain, XmlWriter& xmlWriter) const {
        auto ambientChunk = xmlWriter.createChunk(AMBIENT_TAG, XmlAttribute(), terrainChunk);
        ambientChunk->setFloatValue(terrain->getAmbient());
    }

    void TerrainReaderWriter::loadGrassOn(Terrain* terrain, const XmlChunk* terrainChunk, const XmlParser& xmlParser) const {
        auto grassChunk = xmlParser.getUniqueChunk(false, GRASS_TAG, XmlAttribute(), terrainChunk);
        if (grassChunk) {
            auto grassTextureFilenameChunk = xmlParser.getUniqueChunk(true, GRASS_TEXTURE_FILENAME_TAG, XmlAttribute(), grassChunk.get());
            terrain->getGrass()->setGrassTexture(grassTextureFilenameChunk->getStringValue());

            auto grassMaskFilenameChunk = xmlParser.getUniqueChunk(true, GRASS_MASK_FILENAME_TAG, XmlAttribute(), grassChunk.get());
            terrain->getGrass()->setMaskTexture(grassMaskFilenameChunk->getStringValue());

            auto numGrassInTexChunk = xmlParser.getUniqueChunk(true, NUM_GRASS_IN_TEX_TAG, XmlAttribute(), grassChunk.get());
            terrain->getGrass()->setNumGrassInTexture(numGrassInTexChunk->getUnsignedIntValue());

            auto grassHeightChunk = xmlParser.getUniqueChunk(true, GRASS_HEIGHT_TAG, XmlAttribute(), grassChunk.get());
            terrain->getGrass()->setGrassHeight(grassHeightChunk->getFloatValue());

            auto grassLengthChunk = xmlParser.getUniqueChunk(true, GRASS_LENGTH_TAG, XmlAttribute(), grassChunk.get());
            terrain->getGrass()->setGrassLength(grassLengthChunk->getFloatValue());

            auto grassQuantityChunk = xmlParser.getUniqueChunk(true, GRASS_QUANTITY_TAG, XmlAttribute(), grassChunk.get());
            terrain->getGrass()->setGrassQuantity(grassQuantityChunk->getFloatValue());

            auto windDirectionChunk = xmlParser.getUniqueChunk(true, WIND_DIRECTION_TAG, XmlAttribute(), grassChunk.get());
            terrain->getGrass()->setWindDirection(windDirectionChunk->getVector3Value());

            auto windStrengthChunk = xmlParser.getUniqueChunk(true, WIND_STRENGTH_TAG, XmlAttribute(), grassChunk.get());
            terrain->getGrass()->setWindStrength(windStrengthChunk->getFloatValue());
        }
    }

    void TerrainReaderWriter::writeGrassOn(const XmlChunk* terrainChunk, const Terrain* terrain, XmlWriter& xmlWriter) const {
        if (terrain->getGrass()) {
            auto grassChunk = xmlWriter.createChunk(GRASS_TAG, XmlAttribute(), terrainChunk);

            auto grassTextureFilenameChunk = xmlWriter.createChunk(GRASS_TEXTURE_FILENAME_TAG, XmlAttribute(), grassChunk.get());
            grassTextureFilenameChunk->setStringValue(terrain->getGrass()->getGrassTexture());

            auto grassMaskFilenameChunk = xmlWriter.createChunk(GRASS_MASK_FILENAME_TAG, XmlAttribute(), grassChunk.get());
            grassMaskFilenameChunk->setStringValue(terrain->getGrass()->getMaskTexture());

            auto numGrassInTexChunk = xmlWriter.createChunk(NUM_GRASS_IN_TEX_TAG, XmlAttribute(), grassChunk.get());
            numGrassInTexChunk->setUnsignedIntValue(terrain->getGrass()->getNumGrassInTexture());

            auto grassHeightChunk = xmlWriter.createChunk(GRASS_HEIGHT_TAG, XmlAttribute(), grassChunk.get());
            grassHeightChunk->setFloatValue(terrain->getGrass()->getGrassHeight());

            auto grassLengthChunk = xmlWriter.createChunk(GRASS_LENGTH_TAG, XmlAttribute(), grassChunk.get());
            grassLengthChunk->setFloatValue(terrain->getGrass()->getGrassLength());

            auto grassQuantityChunk = xmlWriter.createChunk(GRASS_QUANTITY_TAG, XmlAttribute(), grassChunk.get());
            grassQuantityChunk->setFloatValue(terrain->getGrass()->getGrassQuantity());

            auto windDirectionChunk = xmlWriter.createChunk(WIND_DIRECTION_TAG, XmlAttribute(), grassChunk.get());
            windDirectionChunk->setVector3Value(terrain->getGrass()->getWindDirection());

            auto windStrengthChunk = xmlWriter.createChunk(WIND_STRENGTH_TAG, XmlAttribute(), grassChunk.get());
            windStrengthChunk->setFloatValue(terrain->getGrass()->getWindStrength());
        }
    }
}
