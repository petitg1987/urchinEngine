#include <map/save/water/WaterEntityReaderWriter.h>

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

    void WaterEntityReaderWriter::write(UdaChunk& waterEntityChunk, const WaterEntity& waterEntity, UdaWriter& udaWriter) {
        waterEntityChunk.addAttribute(UdaAttribute(NAME_ATTR, waterEntity.getName()));

        writeGeneralProperties(waterEntityChunk, *waterEntity.getWater(), udaWriter);
        writeWaterSurfaceProperties(waterEntityChunk, *waterEntity.getWater(), udaWriter);
        writeUnderWaterProperties(waterEntityChunk, *waterEntity.getWater(), udaWriter);
    }

    void WaterEntityReaderWriter::loadGeneralProperties(Water& water, const UdaChunk* waterEntityChunk, const UdaParser& udaParser) {
        auto centerPositionChunk = udaParser.getUniqueChunk(true, CENTER_POSITION_TAG, UdaAttribute(), waterEntityChunk);
        water.setCenterPosition(centerPositionChunk->getPoint3Value());

        auto xSizeChunk = udaParser.getUniqueChunk(true, X_SIZE_TAG, UdaAttribute(), waterEntityChunk);
        water.setXSize(xSizeChunk->getFloatValue());

        auto zSizeChunk = udaParser.getUniqueChunk(true, Z_SIZE_TAG, UdaAttribute(), waterEntityChunk);
        water.setZSize(zSizeChunk->getFloatValue());
    }

    void WaterEntityReaderWriter::writeGeneralProperties(UdaChunk& waterEntityChunk, const Water& water, UdaWriter& udaWriter) {
        auto& centerPositionChunk = udaWriter.createChunk(CENTER_POSITION_TAG, UdaAttribute(), &waterEntityChunk);
        centerPositionChunk.setPoint3Value(water.getCenterPosition());

        auto& xSizeChunk = udaWriter.createChunk(X_SIZE_TAG, UdaAttribute(), &waterEntityChunk);
        xSizeChunk.setFloatValue(water.getXSize());

        auto& zSizeChunk = udaWriter.createChunk(Z_SIZE_TAG, UdaAttribute(), &waterEntityChunk);
        zSizeChunk.setFloatValue(water.getZSize());
    }

    void WaterEntityReaderWriter::loadWaterSurfaceProperties(Water& water, const UdaChunk* waterEntityChunk, const UdaParser& udaParser) {
        auto waterColorChunk = udaParser.getUniqueChunk(true, WATER_COLOR_TAG, UdaAttribute(), waterEntityChunk);
        water.setWaterColor(waterColorChunk->getVector3Value());

        auto normalFilenameChunk = udaParser.getUniqueChunk(true, NORMAL_FILENAME_TAG, UdaAttribute(), waterEntityChunk);
        water.setNormalTexture(normalFilenameChunk->getStringValue());

        auto dudvMapFilenameChunk = udaParser.getUniqueChunk(true, DUDV_MAP_FILENAME_TAG, UdaAttribute(), waterEntityChunk);
        water.setDudvMap(dudvMapFilenameChunk->getStringValue());

        auto waveSpeedChunk = udaParser.getUniqueChunk(true, WAVE_SPEED_TAG, UdaAttribute(), waterEntityChunk);
        water.setWaveSpeed(waveSpeedChunk->getFloatValue());

        auto waveStrengthChunk = udaParser.getUniqueChunk(true, WAVE_STRENGTH_TAG, UdaAttribute(), waterEntityChunk);
        water.setWaveStrength(waveStrengthChunk->getFloatValue());

        auto sRepeatChunk = udaParser.getUniqueChunk(true, S_REPEAT_TAG, UdaAttribute(), waterEntityChunk);
        water.setSRepeat(sRepeatChunk->getFloatValue());

        auto tRepeatChunk = udaParser.getUniqueChunk(true, T_REPEAT_TAG, UdaAttribute(), waterEntityChunk);
        water.setTRepeat(tRepeatChunk->getFloatValue());
    }

    void WaterEntityReaderWriter::writeWaterSurfaceProperties(UdaChunk& waterEntityChunk, const Water& water, UdaWriter& udaWriter) {
        auto& waterColorChunk = udaWriter.createChunk(WATER_COLOR_TAG, UdaAttribute(), &waterEntityChunk);
        waterColorChunk.setVector3Value(water.getWaterColor());

        auto& normalFilenameChunk = udaWriter.createChunk(NORMAL_FILENAME_TAG, UdaAttribute(), &waterEntityChunk);
        normalFilenameChunk.setStringValue(water.getNormalFilename());

        auto& dudvMapFilenameChunk = udaWriter.createChunk(DUDV_MAP_FILENAME_TAG, UdaAttribute(), &waterEntityChunk);
        dudvMapFilenameChunk.setStringValue(water.getDudvFilename());

        auto& waveSpeedChunk = udaWriter.createChunk(WAVE_SPEED_TAG, UdaAttribute(), &waterEntityChunk);
        waveSpeedChunk.setFloatValue(water.getWaveSpeed());

        auto& waveStrengthChunk = udaWriter.createChunk(WAVE_STRENGTH_TAG, UdaAttribute(), &waterEntityChunk);
        waveStrengthChunk.setFloatValue(water.getWaveStrength());

        auto& sRepeatChunk = udaWriter.createChunk(S_REPEAT_TAG, UdaAttribute(), &waterEntityChunk);
        sRepeatChunk.setFloatValue(water.getSRepeat());

        auto& tRepeatChunk = udaWriter.createChunk(T_REPEAT_TAG, UdaAttribute(), &waterEntityChunk);
        tRepeatChunk.setFloatValue(water.getTRepeat());
    }

    void WaterEntityReaderWriter::loadUnderWaterProperties(Water& water, const UdaChunk* waterEntityChunk, const UdaParser& udaParser) {
        auto densityChunk = udaParser.getUniqueChunk(true, DENSITY_TAG, UdaAttribute(), waterEntityChunk);
        water.setDensity(densityChunk->getFloatValue());

        auto gradientChunk = udaParser.getUniqueChunk(true, GRADIENT_TAG, UdaAttribute(), waterEntityChunk);
        water.setGradient(gradientChunk->getFloatValue());
    }

    void WaterEntityReaderWriter::writeUnderWaterProperties(UdaChunk& waterEntityChunk, const Water& water, UdaWriter& udaWriter) {
        auto& densityChunk = udaWriter.createChunk(DENSITY_TAG, UdaAttribute(), &waterEntityChunk);
        densityChunk.setFloatValue(water.getDensity());

        auto& gradientChunk = udaWriter.createChunk(GRADIENT_TAG, UdaAttribute(), &waterEntityChunk);
        gradientChunk.setFloatValue(water.getGradient());
    }

}
