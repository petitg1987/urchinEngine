#include <map/save/terrain/TerrainEntityReaderWriter.h>
#include <util/PathUtil.h>

namespace urchin {

    std::unique_ptr<TerrainEntity> TerrainEntityReaderWriter::load(const UdaChunk* terrainEntityChunk, const UdaParser& udaParser) {
        auto terrainEntity = std::make_unique<TerrainEntity>();

        std::unique_ptr<Terrain> terrain = buildTerrain(terrainEntityChunk, udaParser);
        loadProperties(*terrain, terrainEntityChunk, udaParser);
        loadGrass(*terrain, terrainEntityChunk, udaParser);
        auto collisionTerrainShape = std::make_unique<CollisionHeightfieldShape>(terrain->getMesh()->getVertices(), terrain->getMesh()->getXSize(), terrain->getMesh()->getZSize());
        auto terrainRigidBody = std::make_unique<RigidBody>(terrainEntity->getName(), PhysicsTransform(terrain->getPosition()), std::move(collisionTerrainShape));

        terrainEntity->setName(terrainEntityChunk->getAttributeValue(NAME_ATTR));
        terrainEntity->setTerrain(std::move(terrain));
        terrainEntity->setupInteractiveBody(std::move(terrainRigidBody));

        return terrainEntity;
    }

    void TerrainEntityReaderWriter::write(UdaChunk& terrainEntityChunk, const TerrainEntity& terrainEntity, UdaWriter& udaWriter) {
        terrainEntityChunk.addAttribute(UdaAttribute(NAME_ATTR, terrainEntity.getName()));

        writeTerrainChunk(terrainEntityChunk, *terrainEntity.getTerrain(), udaWriter);
        writeProperties(terrainEntityChunk, *terrainEntity.getTerrain(), udaWriter);
        writeGrass(terrainEntityChunk, *terrainEntity.getTerrain(), udaWriter);
    }

    std::unique_ptr<Terrain> TerrainEntityReaderWriter::buildTerrain(const UdaChunk* terrainEntityChunk, const UdaParser& udaParser) {
        auto meshChunk = udaParser.getFirstChunk(true, MESH_TAG, UdaAttribute(), terrainEntityChunk);
        auto heightFilenameChunk = udaParser.getFirstChunk(true, HEIGHT_FILENAME_TAG, UdaAttribute(), meshChunk);
        auto xzScaleChunk = udaParser.getFirstChunk(true, XZ_SCALE_TAG, UdaAttribute(), meshChunk);
        auto yScaleChunk = udaParser.getFirstChunk(true, Y_SCALE_TAG, UdaAttribute(), meshChunk);
        auto terrainMesh = std::make_unique<TerrainMesh>(heightFilenameChunk->getStringValue(), xzScaleChunk->getFloatValue(), yScaleChunk->getFloatValue());

        auto materialChunk = udaParser.getFirstChunk(true, MATERIAL_TAG, UdaAttribute(), terrainEntityChunk);
        auto maskMapFilenameChunk = udaParser.getFirstChunk(true, MASK_MAP_FILENAME, UdaAttribute(), materialChunk);
        auto sRepeatChunk = udaParser.getFirstChunk(true, S_REPEAT_TAG, UdaAttribute(), materialChunk);
        auto tRepeatChunk = udaParser.getFirstChunk(true, T_REPEAT_TAG, UdaAttribute(), materialChunk);
        auto materialFilenamesChunk = udaParser.getFirstChunk(true, MATERIAL_FILENAMES, UdaAttribute(), materialChunk);
        std::vector<std::string> materialFilenames;
        for (unsigned int i = 0; i < TerrainMaterials::MAX_MATERIAL; ++i) {
            auto materialFilenameChunk = udaParser.getFirstChunk(false, MATERIAL_FILENAME, UdaAttribute(INDEX_ATTR, std::to_string(i)), materialFilenamesChunk);
            if (materialFilenameChunk) {
                materialFilenames.emplace_back(materialFilenameChunk->getStringValue());
            } else {
                materialFilenames.emplace_back("");
            }
        }
        auto terrainMaterial = std::make_unique<TerrainMaterials>(maskMapFilenameChunk->getStringValue(), materialFilenames, sRepeatChunk->getFloatValue(), tRepeatChunk->getFloatValue());

        auto positionChunk = udaParser.getFirstChunk(true, POSITION_TAG, UdaAttribute(), terrainEntityChunk);

        return std::make_unique<Terrain>(std::move(terrainMesh), std::move(terrainMaterial), positionChunk->getPoint3Value());
    }

    void TerrainEntityReaderWriter::writeTerrainChunk(UdaChunk& terrainEntityChunk, const Terrain& terrain, UdaWriter& udaWriter) {
        auto& meshChunk = udaWriter.createChunk(MESH_TAG, UdaAttribute(), &terrainEntityChunk);
        std::string relativeHeightFilename = PathUtil::computeRelativePath(FileSystem::instance().getResourcesDirectory(), terrain.getMesh()->getHeightFilename());
        udaWriter.createChunk(HEIGHT_FILENAME_TAG, UdaAttribute(), &meshChunk).setStringValue(relativeHeightFilename);
        udaWriter.createChunk(XZ_SCALE_TAG, UdaAttribute(), &meshChunk).setFloatValue(terrain.getMesh()->getXZScale());
        udaWriter.createChunk(Y_SCALE_TAG, UdaAttribute(), &meshChunk).setFloatValue(terrain.getMesh()->getYScale());

        auto& materialChunk = udaWriter.createChunk(MATERIAL_TAG, UdaAttribute(), &terrainEntityChunk);
        std::string relativeMaskMapFilename = PathUtil::computeRelativePath(FileSystem::instance().getResourcesDirectory(), terrain.getMaterials()->getMaskMapFilename());
        udaWriter.createChunk(MASK_MAP_FILENAME, UdaAttribute(), &materialChunk).setStringValue(relativeMaskMapFilename);
        udaWriter.createChunk(S_REPEAT_TAG, UdaAttribute(), &materialChunk).setFloatValue(terrain.getMaterials()->getStRepeat().X);
        udaWriter.createChunk(T_REPEAT_TAG, UdaAttribute(), &materialChunk).setFloatValue(terrain.getMaterials()->getStRepeat().Y);
        auto& materialFilenamesChunk = udaWriter.createChunk(MATERIAL_FILENAMES, UdaAttribute(), &materialChunk);
        unsigned int i = 0;
        for (const auto& material : terrain.getMaterials()->getMaterials()) {
            if (material != nullptr) {
                std::string relativeMaterialFilename = PathUtil::computeRelativePath(FileSystem::instance().getResourcesDirectory(), material->getName());
                udaWriter.createChunk(MATERIAL_FILENAME, UdaAttribute(INDEX_ATTR, std::to_string(i)), &materialFilenamesChunk).setStringValue(relativeMaterialFilename);
            }
            ++i;
        }

        udaWriter.createChunk(POSITION_TAG, UdaAttribute(), &terrainEntityChunk).setPoint3Value(terrain.getPosition());
    }

    void TerrainEntityReaderWriter::loadProperties(Terrain& terrain, const UdaChunk* terrainChunk, const UdaParser& udaParser) {
        auto ambientChunk = udaParser.getFirstChunk(true, AMBIENT_TAG, UdaAttribute(), terrainChunk);
        terrain.setAmbient(ambientChunk->getFloatValue());
    }

    void TerrainEntityReaderWriter::writeProperties(UdaChunk& terrainEntityChunk, const Terrain& terrain, UdaWriter& udaWriter) {
        auto& ambientChunk = udaWriter.createChunk(AMBIENT_TAG, UdaAttribute(), &terrainEntityChunk);
        ambientChunk.setFloatValue(terrain.getAmbient());
    }

    void TerrainEntityReaderWriter::loadGrass(Terrain& terrain, const UdaChunk* terrainChunk, const UdaParser& udaParser) {
        auto grassChunk = udaParser.getFirstChunk(false, GRASS_TAG, UdaAttribute(), terrainChunk);
        if (grassChunk) {
            auto grassTextureFilenameChunk = udaParser.getFirstChunk(true, GRASS_TEXTURE_FILENAME_TAG, UdaAttribute(), grassChunk);
            terrain.getGrass().setGrassTexture(grassTextureFilenameChunk->getStringValue());

            auto grassMaskFilenameChunk = udaParser.getFirstChunk(true, GRASS_MASK_FILENAME_TAG, UdaAttribute(), grassChunk);
            terrain.getGrass().setMaskTexture(grassMaskFilenameChunk->getStringValue());

            auto numGrassInTexChunk = udaParser.getFirstChunk(true, NUM_GRASS_IN_TEX_TAG, UdaAttribute(), grassChunk);
            terrain.getGrass().setNumGrassInTexture(numGrassInTexChunk->getUnsignedIntValue());

            auto grassHeightChunk = udaParser.getFirstChunk(true, GRASS_HEIGHT_TAG, UdaAttribute(), grassChunk);
            terrain.getGrass().setGrassHeight(grassHeightChunk->getFloatValue());

            auto grassLengthChunk = udaParser.getFirstChunk(true, GRASS_LENGTH_TAG, UdaAttribute(), grassChunk);
            terrain.getGrass().setGrassLength(grassLengthChunk->getFloatValue());

            auto grassQuantityChunk = udaParser.getFirstChunk(true, GRASS_QUANTITY_TAG, UdaAttribute(), grassChunk);
            terrain.getGrass().setGrassQuantity(grassQuantityChunk->getFloatValue());

            auto windDirectionChunk = udaParser.getFirstChunk(true, WIND_DIRECTION_TAG, UdaAttribute(), grassChunk);
            terrain.getGrass().setWindDirection(windDirectionChunk->getVector3Value());

            auto windStrengthChunk = udaParser.getFirstChunk(true, WIND_STRENGTH_TAG, UdaAttribute(), grassChunk);
            terrain.getGrass().setWindStrength(windStrengthChunk->getFloatValue());
        }
    }

    void TerrainEntityReaderWriter::writeGrass(UdaChunk& terrainEntityChunk, const Terrain& terrain, UdaWriter& udaWriter) {
        auto& grassChunk = udaWriter.createChunk(GRASS_TAG, UdaAttribute(), &terrainEntityChunk);

        std::string relativeGrassTexFilename = PathUtil::computeRelativePath(FileSystem::instance().getResourcesDirectory(), terrain.getGrass().getGrassTexture());
        udaWriter.createChunk(GRASS_TEXTURE_FILENAME_TAG, UdaAttribute(), &grassChunk).setStringValue(relativeGrassTexFilename);

        std::string relativeGrassMaskFilename = PathUtil::computeRelativePath(FileSystem::instance().getResourcesDirectory(), terrain.getGrass().getMaskTexture());
        udaWriter.createChunk(GRASS_MASK_FILENAME_TAG, UdaAttribute(), &grassChunk).setStringValue(relativeGrassMaskFilename);

        udaWriter.createChunk(NUM_GRASS_IN_TEX_TAG, UdaAttribute(), &grassChunk).setUnsignedIntValue(terrain.getGrass().getNumGrassInTexture());

        udaWriter.createChunk(GRASS_HEIGHT_TAG, UdaAttribute(), &grassChunk).setFloatValue(terrain.getGrass().getGrassHeight());

        udaWriter.createChunk(GRASS_LENGTH_TAG, UdaAttribute(), &grassChunk).setFloatValue(terrain.getGrass().getGrassLength());

        udaWriter.createChunk(GRASS_QUANTITY_TAG, UdaAttribute(), &grassChunk).setFloatValue(terrain.getGrass().getGrassQuantity());

        udaWriter.createChunk(WIND_DIRECTION_TAG, UdaAttribute(), &grassChunk).setVector3Value(terrain.getGrass().getWindDirection());

        udaWriter.createChunk(WIND_STRENGTH_TAG, UdaAttribute(), &grassChunk).setFloatValue(terrain.getGrass().getWindStrength());
    }
}
