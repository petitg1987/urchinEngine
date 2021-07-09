#include "WaterReaderWriter.h"

namespace urchin {
    Water* WaterReaderWriter::loadFrom(const XmlChunk* waterChunk, const XmlParser& xmlParser) const {
        auto* water = new Water();
        loadGeneralPropertiesOn(water, waterChunk, xmlParser);
        loadWaterSurfaceProperties(water, waterChunk, xmlParser);
        loadUnderWaterProperties(water, waterChunk, xmlParser);

        return water;
    }

    void WaterReaderWriter::writeOn(XmlChunk* waterChunk, const Water* water, XmlWriter& xmlWriter) const {
        writeGeneralPropertiesOn(waterChunk, water, xmlWriter);
        writeWaterSurfacePropertiesOn(waterChunk, water, xmlWriter);
        writeUnderWaterPropertiesOn(waterChunk, water, xmlWriter);
    }

    void WaterReaderWriter::loadGeneralPropertiesOn(Water* water, const XmlChunk* waterChunk, const XmlParser& xmlParser) {
        auto centerPositionChunk = xmlParser.getUniqueChunk(true, CENTER_POSITION_TAG, XmlAttribute(), waterChunk);
        water->setCenterPosition(centerPositionChunk->getPoint3Value());

        auto xSizeChunk = xmlParser.getUniqueChunk(true, X_SIZE_TAG, XmlAttribute(), waterChunk);
        water->setXSize(xSizeChunk->getFloatValue());

        auto zSizeChunk = xmlParser.getUniqueChunk(true, Z_SIZE_TAG, XmlAttribute(), waterChunk);
        water->setZSize(zSizeChunk->getFloatValue());
    }

    void WaterReaderWriter::writeGeneralPropertiesOn(XmlChunk* waterChunk, const Water* water, XmlWriter& xmlWriter) {
        auto centerPositionChunk = xmlWriter.createChunk(CENTER_POSITION_TAG, XmlAttribute(), waterChunk);
        centerPositionChunk->setPoint3Value(water->getCenterPosition());

        auto xSizeChunk = xmlWriter.createChunk(X_SIZE_TAG, XmlAttribute(), waterChunk);
        xSizeChunk->setFloatValue(water->getXSize());

        auto zSizeChunk = xmlWriter.createChunk(Z_SIZE_TAG, XmlAttribute(), waterChunk);
        zSizeChunk->setFloatValue(water->getZSize());
    }

    void WaterReaderWriter::loadWaterSurfaceProperties(Water* water, const XmlChunk* waterChunk, const XmlParser& xmlParser) {
        auto waterColorChunk = xmlParser.getUniqueChunk(true, WATER_COLOR_TAG, XmlAttribute(), waterChunk);
        water->setWaterColor(waterColorChunk->getVector3Value());

        auto normalFilenameChunk = xmlParser.getUniqueChunk(true, NORMAL_FILENAME_TAG, XmlAttribute(), waterChunk);
        water->setNormalTexture(normalFilenameChunk->getStringValue());

        auto dudvMapFilenameChunk = xmlParser.getUniqueChunk(true, DUDV_MAP_FILENAME_TAG, XmlAttribute(), waterChunk);
        water->setDudvMap(dudvMapFilenameChunk->getStringValue());

        auto waveSpeedChunk = xmlParser.getUniqueChunk(true, WAVE_SPEED_TAG, XmlAttribute(), waterChunk);
        water->setWaveSpeed(waveSpeedChunk->getFloatValue());

        auto waveStrengthChunk = xmlParser.getUniqueChunk(true, WAVE_STRENGTH_TAG, XmlAttribute(), waterChunk);
        water->setWaveStrength(waveStrengthChunk->getFloatValue());

        auto sRepeatChunk = xmlParser.getUniqueChunk(true, S_REPEAT_TAG, XmlAttribute(), waterChunk);
        water->setSRepeat(sRepeatChunk->getFloatValue());

        auto tRepeatChunk = xmlParser.getUniqueChunk(true, T_REPEAT_TAG, XmlAttribute(), waterChunk);
        water->setTRepeat(tRepeatChunk->getFloatValue());
    }

    void WaterReaderWriter::writeWaterSurfacePropertiesOn(XmlChunk* waterChunk, const Water* water, XmlWriter& xmlWriter) {
        auto waterColorChunk = xmlWriter.createChunk(WATER_COLOR_TAG, XmlAttribute(), waterChunk);
        waterColorChunk->setVector3Value(water->getWaterColor());

        auto normalFilenameChunk = xmlWriter.createChunk(NORMAL_FILENAME_TAG, XmlAttribute(), waterChunk);
        normalFilenameChunk->setStringValue(water->getNormalFilename());

        auto dudvMapFilenameChunk = xmlWriter.createChunk(DUDV_MAP_FILENAME_TAG, XmlAttribute(), waterChunk);
        dudvMapFilenameChunk->setStringValue(water->getDudvFilename());

        auto waveSpeedChunk = xmlWriter.createChunk(WAVE_SPEED_TAG, XmlAttribute(), waterChunk);
        waveSpeedChunk->setFloatValue(water->getWaveSpeed());

        auto waveStrengthChunk = xmlWriter.createChunk(WAVE_STRENGTH_TAG, XmlAttribute(), waterChunk);
        waveStrengthChunk->setFloatValue(water->getWaveStrength());

        auto sRepeatChunk = xmlWriter.createChunk(S_REPEAT_TAG, XmlAttribute(), waterChunk);
        sRepeatChunk->setFloatValue(water->getSRepeat());

        auto tRepeatChunk = xmlWriter.createChunk(T_REPEAT_TAG, XmlAttribute(), waterChunk);
        tRepeatChunk->setFloatValue(water->getTRepeat());
    }

    void WaterReaderWriter::loadUnderWaterProperties(Water* water, const XmlChunk* waterChunk, const XmlParser& xmlParser) {
        auto densityChunk = xmlParser.getUniqueChunk(true, DENSITY_TAG, XmlAttribute(), waterChunk);
        water->setDensity(densityChunk->getFloatValue());

        auto gradientChunk = xmlParser.getUniqueChunk(true, GRADIENT_TAG, XmlAttribute(), waterChunk);
        water->setGradient(gradientChunk->getFloatValue());
    }

    void WaterReaderWriter::writeUnderWaterPropertiesOn(XmlChunk* waterChunk, const Water* water, XmlWriter& xmlWriter) {
        auto densityChunk = xmlWriter.createChunk(DENSITY_TAG, XmlAttribute(), waterChunk);
        densityChunk->setFloatValue(water->getDensity());

        auto gradientChunk = xmlWriter.createChunk(GRADIENT_TAG, XmlAttribute(), waterChunk);
        gradientChunk->setFloatValue(water->getGradient());
    }

}
