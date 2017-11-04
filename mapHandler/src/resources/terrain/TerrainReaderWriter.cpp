#include "TerrainReaderWriter.h"

namespace urchin
{
    Terrain *TerrainReaderWriter::loadFrom(std::shared_ptr<XmlChunk> terrainChunk, const XmlParser &xmlParser) const
    {
        Terrain *terrain = buildTerrainFrom(terrainChunk, xmlParser);

        loadPropertiesOn(terrain, terrainChunk, xmlParser);

        return terrain;
    }

    void TerrainReaderWriter::writeOn(std::shared_ptr<XmlChunk> terrainChunk, const Terrain *terrain, XmlWriter &xmlWriter) const
    {
        buildChunkFrom(terrainChunk, terrain, xmlWriter);

        writePropertiesOn(terrainChunk, terrain, xmlWriter);
    }

    Terrain *TerrainReaderWriter::buildTerrainFrom(std::shared_ptr<XmlChunk> terrainChunk, const XmlParser &xmlParser) const
    {
        std::shared_ptr<XmlChunk> meshChunk = xmlParser.getUniqueChunk(true, MESH_TAG, XmlAttribute(), terrainChunk);
        std::shared_ptr<XmlChunk> heightFilenameChunk = xmlParser.getUniqueChunk(true, HEIGHT_FILENAME_TAG, XmlAttribute(), meshChunk);
        std::shared_ptr<XmlChunk> xzScaleChunk = xmlParser.getUniqueChunk(true, XZ_SCALE_TAG, XmlAttribute(), meshChunk);
        std::shared_ptr<XmlChunk> yScaleChunk = xmlParser.getUniqueChunk(true, Y_SCALE_TAG, XmlAttribute(), meshChunk);
        auto terrainMesh = std::make_unique<TerrainMesh>(heightFilenameChunk->getStringValue(), xzScaleChunk->getFloatValue(), yScaleChunk->getFloatValue());

        std::shared_ptr<XmlChunk> materialChunk = xmlParser.getUniqueChunk(true, MATERIAL_TAG, XmlAttribute(), terrainChunk);
        std::shared_ptr<XmlChunk> maskMapFilenameChunk = xmlParser.getUniqueChunk(true, MASK_MAP_FILENAME, XmlAttribute(), materialChunk);
        std::shared_ptr<XmlChunk> sRepeatChunk = xmlParser.getUniqueChunk(true, S_REPEAT_TAG, XmlAttribute(), materialChunk);
        std::shared_ptr<XmlChunk> tRepeatChunk = xmlParser.getUniqueChunk(true, T_REPEAT_TAG, XmlAttribute(), materialChunk);
        std::shared_ptr<XmlChunk> materialFilenamesChunk = xmlParser.getUniqueChunk(true, MATERIAL_FILENAMES, XmlAttribute(), materialChunk);
        auto terrainMaterial = std::make_unique<TerrainMaterial>(maskMapFilenameChunk->getStringValue(), sRepeatChunk->getFloatValue(), tRepeatChunk->getFloatValue());
        for(unsigned int i=0; i<4; ++i)
        {
            std::shared_ptr<XmlChunk> materialFilenameChunk = xmlParser.getUniqueChunk(false, MATERIAL_FILENAME, XmlAttribute(INDEX_ATTR, std::to_string(i)), materialFilenamesChunk);
            if(materialFilenameChunk!=nullptr)
            {
                terrainMaterial->addMaterial(i, materialFilenameChunk->getStringValue());
            }
        }

        return new Terrain(terrainMesh, terrainMaterial);
    }

    void TerrainReaderWriter::buildChunkFrom(std::shared_ptr<XmlChunk> terrainChunk, const Terrain *terrain, XmlWriter &xmlWriter) const
    {
        std::shared_ptr<XmlChunk> meshChunk = xmlWriter.createChunk(MESH_TAG, XmlAttribute(), terrainChunk);
        std::shared_ptr<XmlChunk> heightFilenameChunk = xmlWriter.createChunk(HEIGHT_FILENAME_TAG, XmlAttribute(), meshChunk);
        heightFilenameChunk->setStringValue(terrain->getMesh()->getHeightFilename());
        std::shared_ptr<XmlChunk> xzScaleChunk = xmlWriter.createChunk(XZ_SCALE_TAG, XmlAttribute(), meshChunk);
        xzScaleChunk->setFloatValue(terrain->getMesh()->getXZScale());
        std::shared_ptr<XmlChunk> yScaleChunk = xmlWriter.createChunk(Y_SCALE_TAG, XmlAttribute(), meshChunk);
        yScaleChunk->setFloatValue(terrain->getMesh()->getYScale());

        std::shared_ptr<XmlChunk> materialChunk = xmlWriter.createChunk(MATERIAL_TAG, XmlAttribute(), terrainChunk);
        std::shared_ptr<XmlChunk> maskMapFilenameChunk = xmlWriter.createChunk(MASK_MAP_FILENAME, XmlAttribute(), materialChunk);
        maskMapFilenameChunk->setStringValue(terrain->getMaterial()->getMaskMapFilename());
        std::shared_ptr<XmlChunk> sRepeatChunk = xmlWriter.createChunk(S_REPEAT_TAG, XmlAttribute(), materialChunk);
        sRepeatChunk->setFloatValue(terrain->getMaterial()->getSRepeat());
        std::shared_ptr<XmlChunk> tRepeatChunk = xmlWriter.createChunk(T_REPEAT_TAG, XmlAttribute(), materialChunk);
        tRepeatChunk->setFloatValue(terrain->getMaterial()->getTRepeat());
        std::shared_ptr<XmlChunk> materialFilenamesChunk = xmlWriter.createChunk(MATERIAL_FILENAMES, XmlAttribute(), materialChunk);
        unsigned int i=0;
        for(const Material *material : terrain->getMaterial()->getMaterials())
        {
            if(material != nullptr)
            {
                std::shared_ptr<XmlChunk> materialFilenameChunk = xmlWriter.createChunk(MATERIAL_FILENAME, XmlAttribute(INDEX_ATTR, std::to_string(i)), materialFilenamesChunk);
                materialFilenameChunk->setStringValue(material->getDiffuseTexture()->getName());
            }
            ++i;
        }
    }

    void TerrainReaderWriter::loadPropertiesOn(Terrain *terrain, std::shared_ptr<XmlChunk> terrainChunk, const XmlParser &xmlParser) const
    {
        std::shared_ptr<XmlChunk> positionChunk = xmlParser.getUniqueChunk(true, POSITION_TAG, XmlAttribute(), terrainChunk);
        terrain->setPosition(positionChunk->getPoint3Value());

        std::shared_ptr<XmlChunk> ambientChunk = xmlParser.getUniqueChunk(true, AMBIENT_TAG, XmlAttribute(), terrainChunk);
        terrain->setAmbient(ambientChunk->getFloatValue());
    }

    void TerrainReaderWriter::writePropertiesOn(std::shared_ptr<XmlChunk> terrainChunk, const Terrain *terrain, XmlWriter &xmlWriter) const
    {
        std::shared_ptr<XmlChunk> positionChunk = xmlWriter.createChunk(POSITION_TAG, XmlAttribute(), terrainChunk);
        positionChunk->setPoint3Value(terrain->getPosition());

        std::shared_ptr<XmlChunk> ambientChunk = xmlWriter.createChunk(AMBIENT_TAG, XmlAttribute(), terrainChunk);
        ambientChunk->setFloatValue(terrain->getAmbient());
    }
}
