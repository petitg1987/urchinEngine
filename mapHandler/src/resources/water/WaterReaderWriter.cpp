#include "WaterReaderWriter.h"

namespace urchin {
    std::unique_ptr<Water> WaterReaderWriter::loadFrom(const UdaChunk* waterChunk, const UdaParser& udaParser) const {
        auto water = std::make_unique<Water>();
        loadGeneralPropertiesOn(*water, waterChunk, udaParser);
        loadWaterSurfaceProperties(*water, waterChunk, udaParser);
        loadUnderWaterProperties(*water, waterChunk, udaParser);

        return water;
    }

    void WaterReaderWriter::writeOn(UdaChunk& waterChunk, const Water& water, UdaWriter& udaWriter) const {
        writeGeneralPropertiesOn(waterChunk, water, udaWriter);
        writeWaterSurfacePropertiesOn(waterChunk, water, udaWriter);
        writeUnderWaterPropertiesOn(waterChunk, water, udaWriter);
    }

    void WaterReaderWriter::loadGeneralPropertiesOn(Water& water, const UdaChunk* waterChunk, const UdaParser& udaParser) {
        auto centerPositionChunk = udaParser.getUniqueChunk(true, CENTER_POSITION_TAG, UdaAttribute(), waterChunk);
        water.setCenterPosition(centerPositionChunk->getPoint3Value());

        auto xSizeChunk = udaParser.getUniqueChunk(true, X_SIZE_TAG, UdaAttribute(), waterChunk);
        water.setXSize(xSizeChunk->getFloatValue());

        auto zSizeChunk = udaParser.getUniqueChunk(true, Z_SIZE_TAG, UdaAttribute(), waterChunk);
        water.setZSize(zSizeChunk->getFloatValue());
    }

    void WaterReaderWriter::writeGeneralPropertiesOn(UdaChunk& waterChunk, const Water& water, UdaWriter& udaWriter) {
        auto& centerPositionChunk = udaWriter.createChunk(CENTER_POSITION_TAG, UdaAttribute(), &waterChunk);
        centerPositionChunk.setPoint3Value(water.getCenterPosition());

        auto& xSizeChunk = udaWriter.createChunk(X_SIZE_TAG, UdaAttribute(), &waterChunk);
        xSizeChunk.setFloatValue(water.getXSize());

        auto& zSizeChunk = udaWriter.createChunk(Z_SIZE_TAG, UdaAttribute(), &waterChunk);
        zSizeChunk.setFloatValue(water.getZSize());
    }

    void WaterReaderWriter::loadWaterSurfaceProperties(Water& water, const UdaChunk* waterChunk, const UdaParser& udaParser) {
        auto waterColorChunk = udaParser.getUniqueChunk(true, WATER_COLOR_TAG, UdaAttribute(), waterChunk);
        water.setWaterColor(waterColorChunk->getVector3Value());

        auto normalFilenameChunk = udaParser.getUniqueChunk(true, NORMAL_FILENAME_TAG, UdaAttribute(), waterChunk);
        water.setNormalTexture(normalFilenameChunk->getStringValue());

        auto dudvMapFilenameChunk = udaParser.getUniqueChunk(true, DUDV_MAP_FILENAME_TAG, UdaAttribute(), waterChunk);
        water.setDudvMap(dudvMapFilenameChunk->getStringValue());

        auto waveSpeedChunk = udaParser.getUniqueChunk(true, WAVE_SPEED_TAG, UdaAttribute(), waterChunk);
        water.setWaveSpeed(waveSpeedChunk->getFloatValue());

        auto waveStrengthChunk = udaParser.getUniqueChunk(true, WAVE_STRENGTH_TAG, UdaAttribute(), waterChunk);
        water.setWaveStrength(waveStrengthChunk->getFloatValue());

        auto sRepeatChunk = udaParser.getUniqueChunk(true, S_REPEAT_TAG, UdaAttribute(), waterChunk);
        water.setSRepeat(sRepeatChunk->getFloatValue());

        auto tRepeatChunk = udaParser.getUniqueChunk(true, T_REPEAT_TAG, UdaAttribute(), waterChunk);
        water.setTRepeat(tRepeatChunk->getFloatValue());
    }

    void WaterReaderWriter::writeWaterSurfacePropertiesOn(UdaChunk& waterChunk, const Water& water, UdaWriter& udaWriter) {
        auto& waterColorChunk = udaWriter.createChunk(WATER_COLOR_TAG, UdaAttribute(), &waterChunk);
        waterColorChunk.setVector3Value(water.getWaterColor());

        auto& normalFilenameChunk = udaWriter.createChunk(NORMAL_FILENAME_TAG, UdaAttribute(), &waterChunk);
        normalFilenameChunk.setStringValue(water.getNormalFilename());

        auto& dudvMapFilenameChunk = udaWriter.createChunk(DUDV_MAP_FILENAME_TAG, UdaAttribute(), &waterChunk);
        dudvMapFilenameChunk.setStringValue(water.getDudvFilename());

        auto& waveSpeedChunk = udaWriter.createChunk(WAVE_SPEED_TAG, UdaAttribute(), &waterChunk);
        waveSpeedChunk.setFloatValue(water.getWaveSpeed());

        auto& waveStrengthChunk = udaWriter.createChunk(WAVE_STRENGTH_TAG, UdaAttribute(), &waterChunk);
        waveStrengthChunk.setFloatValue(water.getWaveStrength());

        auto& sRepeatChunk = udaWriter.createChunk(S_REPEAT_TAG, UdaAttribute(), &waterChunk);
        sRepeatChunk.setFloatValue(water.getSRepeat());

        auto& tRepeatChunk = udaWriter.createChunk(T_REPEAT_TAG, UdaAttribute(), &waterChunk);
        tRepeatChunk.setFloatValue(water.getTRepeat());
    }

    void WaterReaderWriter::loadUnderWaterProperties(Water& water, const UdaChunk* waterChunk, const UdaParser& udaParser) {
        auto densityChunk = udaParser.getUniqueChunk(true, DENSITY_TAG, UdaAttribute(), waterChunk);
        water.setDensity(densityChunk->getFloatValue());

        auto gradientChunk = udaParser.getUniqueChunk(true, GRADIENT_TAG, UdaAttribute(), waterChunk);
        water.setGradient(gradientChunk->getFloatValue());
    }

    void WaterReaderWriter::writeUnderWaterPropertiesOn(UdaChunk& waterChunk, const Water& water, UdaWriter& udaWriter) {
        auto& densityChunk = udaWriter.createChunk(DENSITY_TAG, UdaAttribute(), &waterChunk);
        densityChunk.setFloatValue(water.getDensity());

        auto& gradientChunk = udaWriter.createChunk(GRADIENT_TAG, UdaAttribute(), &waterChunk);
        gradientChunk.setFloatValue(water.getGradient());
    }

}
