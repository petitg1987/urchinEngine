#include "UrchinCommon.h"

#include <character/CharacterControllerConfig.h>

namespace urchin {

    CharacterControllerConfig::CharacterControllerConfig() :
            jumpSpeed(5.0f),
            maxSlopeInRadian(0.0f),
            maxSlopeInPercentage(0.0f),
            timeKeepMoveInAir(2.5f),
            percentageControlInAir(0.4f),
            maxVerticalSpeed(30.0f), //human falling maximum speed is 55 m/s but we use 30 m/s to improve performance of CCD
            maxHorizontalSpeed(15.0f) {
        setMaxSlopeInRadian(MathValue::PI_FLOAT / 4.0f); //45 degrees
    }

    void CharacterControllerConfig::setJumpSpeed(float jumpSpeed) {
        this->jumpSpeed = jumpSpeed;
    }

    float CharacterControllerConfig::getJumpSpeed() const {
        return jumpSpeed;
    }

    void CharacterControllerConfig::setMaxSlopeInRadian(float maxSlopeInRadian) {
        this->maxSlopeInRadian = maxSlopeInRadian;
        this->maxSlopeInPercentage = std::tan(maxSlopeInRadian);
    }

    float CharacterControllerConfig::getMaxSlopeInRadian() const {
        return maxSlopeInRadian;
    }

    float CharacterControllerConfig::getMaxSlopeInPercentage() const {
        return maxSlopeInPercentage;
    }

    void CharacterControllerConfig::setTimeKeepMoveInAir(float timeKeepMoveInAir) {
        this->timeKeepMoveInAir = timeKeepMoveInAir;
    }

    float CharacterControllerConfig::getTimeKeepMoveInAir() const {
        return timeKeepMoveInAir;
    }

    void CharacterControllerConfig::setPercentageControlInAir(float percentageControlInAir) {
        this->percentageControlInAir = percentageControlInAir;
    }

    float CharacterControllerConfig::getPercentageControlInAir() const {
        return percentageControlInAir;
    }

    void CharacterControllerConfig::setMaxVerticalSpeed(float maxVerticalSpeed) {
        this->maxVerticalSpeed = maxVerticalSpeed;
    }

    float CharacterControllerConfig::getMaxVerticalSpeed() const {
        return maxVerticalSpeed;
    }

    void CharacterControllerConfig::setMaxHorizontalSpeed(float maxHorizontalSpeed) {
        this->maxHorizontalSpeed = maxHorizontalSpeed;
    }

    float CharacterControllerConfig::getMaxHorizontalSpeed() const {
        return maxHorizontalSpeed;
    }


}
