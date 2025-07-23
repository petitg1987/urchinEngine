#include "map/save/water/WaterEntityReaderWriter.h"
#include "util/PathUtil.h"

namespace urchin {

    std::unique_ptr<WaterEntity> WaterEntityReaderWriter::load(const UdaChunk* waterEntityChunk, const UdaParser& udaParser) {
        auto waterEntity = std::make_unique<WaterEntity>();

        waterEntity->setName(waterEntityChunk->getAttributeValue(NAME_ATTR));

        auto water = std::make_unique<Water>();
        loadGeneralProperties(*water, waterEntityChunk, udaParser);
        loadWaterSurfaceProperties(*water, waterEntityChunk, udaParser);
        loadUnderWaterProperties(*water, waterEntityChunk, udaParser);
        waterEntity->setWater(std::move(water));

        return waterEntity;
    }

    void WaterEntityReaderWriter::write(UdaChunk& waterEntityChunk, const WaterEntity& waterEntity, UdaParser& udaParser) {
        waterEntityChunk.addAttribute(UdaAttribute(NAME_ATTR, waterEntity.getName()));

        writeGeneralProperties(waterEntityChunk, *waterEntity.getWater(), udaParser);
        writeWaterSurfaceProperties(waterEntityChunk, *waterEntity.getWater(), udaParser);
        writeUnderWaterProperties(waterEntityChunk, *waterEntity.getWater(), udaParser);
    }

    void WaterEntityReaderWriter::loadGeneralProperties(Water& water, const UdaChunk* waterEntityChunk, const UdaParser& udaParser) {
        auto centerPositionChunk = udaParser.getFirstChunk(true, CENTER_POSITION_TAG, UdaAttribute(), waterEntityChunk);
        water.setCenterPosition(centerPositionChunk->getPoint3Value());

        auto xSizeChunk = udaParser.getFirstChunk(true, X_SIZE_TAG, UdaAttribute(), waterEntityChunk);
        water.setXSize(xSizeChunk->getFloatValue());

        auto zSizeChunk = udaParser.getFirstChunk(true, Z_SIZE_TAG, UdaAttribute(), waterEntityChunk);
        water.setZSize(zSizeChunk->getFloatValue());
    }

    void WaterEntityReaderWriter::writeGeneralProperties(UdaChunk& waterEntityChunk, const Water& water, UdaParser& udaParser) {
        udaParser.createChunk(CENTER_POSITION_TAG, UdaAttribute(), &waterEntityChunk).setPoint3Value(water.getCenterPosition());

        udaParser.createChunk(X_SIZE_TAG, UdaAttribute(), &waterEntityChunk).setFloatValue(water.getXSize());

        udaParser.createChunk(Z_SIZE_TAG, UdaAttribute(), &waterEntityChunk).setFloatValue(water.getZSize());
    }

    void WaterEntityReaderWriter::loadWaterSurfaceProperties(Water& water, const UdaChunk* waterEntityChunk, const UdaParser& udaParser) {
        auto waterColorChunk = udaParser.getFirstChunk(true, WATER_COLOR_TAG, UdaAttribute(), waterEntityChunk);
        water.setWaterColor(waterColorChunk->getVector3Value());

        auto normalFilenameChunk = udaParser.getFirstChunk(true, NORMAL_FILENAME_TAG, UdaAttribute(), waterEntityChunk);
        water.setNormalTexture(normalFilenameChunk->getStringValue());

        auto dudvMapFilenameChunk = udaParser.getFirstChunk(true, DUDV_MAP_FILENAME_TAG, UdaAttribute(), waterEntityChunk);
        water.setDudvMap(dudvMapFilenameChunk->getStringValue());

        auto waveSpeedChunk = udaParser.getFirstChunk(true, WAVE_SPEED_TAG, UdaAttribute(), waterEntityChunk);
        water.setWaveSpeed(waveSpeedChunk->getFloatValue());

        auto waveStrengthChunk = udaParser.getFirstChunk(true, WAVE_STRENGTH_TAG, UdaAttribute(), waterEntityChunk);
        water.setWaveStrength(waveStrengthChunk->getFloatValue());

        auto sRepeatChunk = udaParser.getFirstChunk(true, S_REPEAT_TAG, UdaAttribute(), waterEntityChunk);
        water.setSRepeat(sRepeatChunk->getFloatValue());

        auto tRepeatChunk = udaParser.getFirstChunk(true, T_REPEAT_TAG, UdaAttribute(), waterEntityChunk);
        water.setTRepeat(tRepeatChunk->getFloatValue());
    }

    void WaterEntityReaderWriter::writeWaterSurfaceProperties(UdaChunk& waterEntityChunk, const Water& water, UdaParser& udaParser) {
        udaParser.createChunk(WATER_COLOR_TAG, UdaAttribute(), &waterEntityChunk).setVector3Value(water.getWaterColor());

        std::string relativeNormalFilename = PathUtil::computeRelativePath(FileSystem::instance().getResourcesDirectory(), water.getNormalFilename());
        udaParser.createChunk(NORMAL_FILENAME_TAG, UdaAttribute(), &waterEntityChunk).setStringValue(relativeNormalFilename);

        std::string relativeDudvFilename = PathUtil::computeRelativePath(FileSystem::instance().getResourcesDirectory(), water.getDudvFilename());
        udaParser.createChunk(DUDV_MAP_FILENAME_TAG, UdaAttribute(), &waterEntityChunk).setStringValue(relativeDudvFilename);

        udaParser.createChunk(WAVE_SPEED_TAG, UdaAttribute(), &waterEntityChunk).setFloatValue(water.getWaveSpeed());

        udaParser.createChunk(WAVE_STRENGTH_TAG, UdaAttribute(), &waterEntityChunk).setFloatValue(water.getWaveStrength());

        udaParser.createChunk(S_REPEAT_TAG, UdaAttribute(), &waterEntityChunk).setFloatValue(water.getSRepeat());

        udaParser.createChunk(T_REPEAT_TAG, UdaAttribute(), &waterEntityChunk).setFloatValue(water.getTRepeat());
    }

    void WaterEntityReaderWriter::loadUnderWaterProperties(Water& water, const UdaChunk* waterEntityChunk, const UdaParser& udaParser) {
        auto densityChunk = udaParser.getFirstChunk(true, DENSITY_TAG, UdaAttribute(), waterEntityChunk);
        water.setDensity(densityChunk->getFloatValue());

        auto gradientChunk = udaParser.getFirstChunk(true, GRADIENT_TAG, UdaAttribute(), waterEntityChunk);
        water.setGradient(gradientChunk->getFloatValue());
    }

    void WaterEntityReaderWriter::writeUnderWaterProperties(UdaChunk& waterEntityChunk, const Water& water, UdaParser& udaParser) {
        udaParser.createChunk(DENSITY_TAG, UdaAttribute(), &waterEntityChunk).setFloatValue(water.getDensity());

        udaParser.createChunk(GRADIENT_TAG, UdaAttribute(), &waterEntityChunk).setFloatValue(water.getGradient());
    }

}
