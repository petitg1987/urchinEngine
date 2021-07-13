#include "WaterReaderWriter.h"

namespace urchin {
    Water* WaterReaderWriter::loadFrom(const DataChunk* waterChunk, const DataParser& dataParser) const {
        auto* water = new Water();
        loadGeneralPropertiesOn(water, waterChunk, dataParser);
        loadWaterSurfaceProperties(water, waterChunk, dataParser);
        loadUnderWaterProperties(water, waterChunk, dataParser);

        return water;
    }

    void WaterReaderWriter::writeOn(DataChunk& waterChunk, const Water* water, UdaWriter& udaWriter) const {
        writeGeneralPropertiesOn(waterChunk, water, udaWriter);
        writeWaterSurfacePropertiesOn(waterChunk, water, udaWriter);
        writeUnderWaterPropertiesOn(waterChunk, water, udaWriter);
    }

    void WaterReaderWriter::loadGeneralPropertiesOn(Water* water, const DataChunk* waterChunk, const DataParser& dataParser) {
        auto centerPositionChunk = dataParser.getUniqueChunk(true, CENTER_POSITION_TAG, UdaAttribute(), waterChunk);
        water->setCenterPosition(centerPositionChunk->getPoint3Value());

        auto xSizeChunk = dataParser.getUniqueChunk(true, X_SIZE_TAG, UdaAttribute(), waterChunk);
        water->setXSize(xSizeChunk->getFloatValue());

        auto zSizeChunk = dataParser.getUniqueChunk(true, Z_SIZE_TAG, UdaAttribute(), waterChunk);
        water->setZSize(zSizeChunk->getFloatValue());
    }

    void WaterReaderWriter::writeGeneralPropertiesOn(DataChunk& waterChunk, const Water* water, UdaWriter& udaWriter) {
        auto& centerPositionChunk = udaWriter.createChunk(CENTER_POSITION_TAG, UdaAttribute(), &waterChunk);
        centerPositionChunk.setPoint3Value(water->getCenterPosition());

        auto& xSizeChunk = udaWriter.createChunk(X_SIZE_TAG, UdaAttribute(), &waterChunk);
        xSizeChunk.setFloatValue(water->getXSize());

        auto& zSizeChunk = udaWriter.createChunk(Z_SIZE_TAG, UdaAttribute(), &waterChunk);
        zSizeChunk.setFloatValue(water->getZSize());
    }

    void WaterReaderWriter::loadWaterSurfaceProperties(Water* water, const DataChunk* waterChunk, const DataParser& dataParser) {
        auto waterColorChunk = dataParser.getUniqueChunk(true, WATER_COLOR_TAG, UdaAttribute(), waterChunk);
        water->setWaterColor(waterColorChunk->getVector3Value());

        auto normalFilenameChunk = dataParser.getUniqueChunk(true, NORMAL_FILENAME_TAG, UdaAttribute(), waterChunk);
        water->setNormalTexture(normalFilenameChunk->getStringValue());

        auto dudvMapFilenameChunk = dataParser.getUniqueChunk(true, DUDV_MAP_FILENAME_TAG, UdaAttribute(), waterChunk);
        water->setDudvMap(dudvMapFilenameChunk->getStringValue());

        auto waveSpeedChunk = dataParser.getUniqueChunk(true, WAVE_SPEED_TAG, UdaAttribute(), waterChunk);
        water->setWaveSpeed(waveSpeedChunk->getFloatValue());

        auto waveStrengthChunk = dataParser.getUniqueChunk(true, WAVE_STRENGTH_TAG, UdaAttribute(), waterChunk);
        water->setWaveStrength(waveStrengthChunk->getFloatValue());

        auto sRepeatChunk = dataParser.getUniqueChunk(true, S_REPEAT_TAG, UdaAttribute(), waterChunk);
        water->setSRepeat(sRepeatChunk->getFloatValue());

        auto tRepeatChunk = dataParser.getUniqueChunk(true, T_REPEAT_TAG, UdaAttribute(), waterChunk);
        water->setTRepeat(tRepeatChunk->getFloatValue());
    }

    void WaterReaderWriter::writeWaterSurfacePropertiesOn(DataChunk& waterChunk, const Water* water, UdaWriter& udaWriter) {
        auto& waterColorChunk = udaWriter.createChunk(WATER_COLOR_TAG, UdaAttribute(), &waterChunk);
        waterColorChunk.setVector3Value(water->getWaterColor());

        auto& normalFilenameChunk = udaWriter.createChunk(NORMAL_FILENAME_TAG, UdaAttribute(), &waterChunk);
        normalFilenameChunk.setStringValue(water->getNormalFilename());

        auto& dudvMapFilenameChunk = udaWriter.createChunk(DUDV_MAP_FILENAME_TAG, UdaAttribute(), &waterChunk);
        dudvMapFilenameChunk.setStringValue(water->getDudvFilename());

        auto& waveSpeedChunk = udaWriter.createChunk(WAVE_SPEED_TAG, UdaAttribute(), &waterChunk);
        waveSpeedChunk.setFloatValue(water->getWaveSpeed());

        auto& waveStrengthChunk = udaWriter.createChunk(WAVE_STRENGTH_TAG, UdaAttribute(), &waterChunk);
        waveStrengthChunk.setFloatValue(water->getWaveStrength());

        auto& sRepeatChunk = udaWriter.createChunk(S_REPEAT_TAG, UdaAttribute(), &waterChunk);
        sRepeatChunk.setFloatValue(water->getSRepeat());

        auto& tRepeatChunk = udaWriter.createChunk(T_REPEAT_TAG, UdaAttribute(), &waterChunk);
        tRepeatChunk.setFloatValue(water->getTRepeat());
    }

    void WaterReaderWriter::loadUnderWaterProperties(Water* water, const DataChunk* waterChunk, const DataParser& dataParser) {
        auto densityChunk = dataParser.getUniqueChunk(true, DENSITY_TAG, UdaAttribute(), waterChunk);
        water->setDensity(densityChunk->getFloatValue());

        auto gradientChunk = dataParser.getUniqueChunk(true, GRADIENT_TAG, UdaAttribute(), waterChunk);
        water->setGradient(gradientChunk->getFloatValue());
    }

    void WaterReaderWriter::writeUnderWaterPropertiesOn(DataChunk& waterChunk, const Water* water, UdaWriter& udaWriter) {
        auto& densityChunk = udaWriter.createChunk(DENSITY_TAG, UdaAttribute(), &waterChunk);
        densityChunk.setFloatValue(water->getDensity());

        auto& gradientChunk = udaWriter.createChunk(GRADIENT_TAG, UdaAttribute(), &waterChunk);
        gradientChunk.setFloatValue(water->getGradient());
    }

}
