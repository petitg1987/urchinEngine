#include <utility>
#include <UrchinCommon.h>

#include <character/CharacterControllerConfig.h>

namespace urchin {

    CharacterControllerConfig::CharacterControllerConfig() :
            characterEventCallback(std::make_unique<CharacterEventCallback>()),
            jumpSpeed(5.0f),
            maxSlopeInRadian(0.0f),
            maxSlopeInPercentage(0.0f),
            maxSlopeSpeedVariation(0.10f), //10%
            timeKeepMoveInAir(2.5f),
            percentageControlInAir(0.4f),
            maxVerticalSpeed(30.0f), //human falling maximum speed is 55 m/s, but we use 30 m/s to improve performance of CCD
            walkSpeed(5.0f), //18 km/h
            runSpeed(10.0f) //36 km/h
    {
        setMaxSlopeInRadian(MathValue::PI_FLOAT / 4.0f); //45 degrees
    }

    void CharacterControllerConfig::setupEventCallback(std::shared_ptr<CharacterEventCallback> characterEventCallback) {
        this->characterEventCallback = std::move(characterEventCallback);
    }

    CharacterEventCallback& CharacterControllerConfig::getEventCallback() const {
        return *characterEventCallback;
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

    /**
    * Define the maximum speed variation (increase or decrease) in percentage when character moving on a slope.
    */
    void CharacterControllerConfig::setMaxSlopeSpeedVariation(float maxSlopeSpeedVariation) {
        this->maxSlopeSpeedVariation = maxSlopeSpeedVariation;
    }

    float CharacterControllerConfig::getMaxSlopeSpeedVariation() const {
        return maxSlopeSpeedVariation;
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

    /**
     * Define the horizontal walk speed allowed for the character. The character walk speed is higher in downhill.
     */
    void CharacterControllerConfig::setWalkSpeed(float walkSpeed) {
        this->walkSpeed = walkSpeed;
    }

    float CharacterControllerConfig::getWalkSpeed() const {
        return walkSpeed;
    }

    /**
     * Define the horizontal run speed allowed for the character. The character run speed is higher in downhill.
     */
    void CharacterControllerConfig::setRunSpeed(float runSpeed) {
        this->runSpeed = runSpeed;
    }

    float CharacterControllerConfig::getRunSpeed() const {
        return runSpeed;
    }

}
