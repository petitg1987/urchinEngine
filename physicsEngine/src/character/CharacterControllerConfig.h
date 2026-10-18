#pragma once

#include "character/CharacterEventCallback.h"

namespace urchin {

    class CharacterControllerConfig {
        public:
            CharacterControllerConfig();

            void setupEventCallback(std::shared_ptr<CharacterEventCallback>);
            CharacterEventCallback& getEventCallback() const;

            void setJumpSpeed(float);
            float getJumpSpeed() const;

            void setMaxSlopeInRadian(float);
            float getMaxSlopeInRadian() const;
            float getMaxSlopeInPercentage() const;
            void setMaxSlopeSpeedVariation(float);
            float getMaxSlopeSpeedVariation() const;

            void setTimeKeepMoveInAir(float);
            float getTimeKeepMoveInAir() const;
            void setPercentageControlInAir(float);
            float getPercentageControlInAir() const;

            void setMaxVerticalSpeed(float);
            float getMaxVerticalSpeed() const;

            void setMaxWalkSpeed(float);
            float getMaxWalkSpeed() const;

            void setMaxRunSpeed(float);
            float getMaxRunSpeed() const;

        private:
            std::shared_ptr<CharacterEventCallback> characterEventCallback;

            float jumpSpeed;

            float maxSlopeInRadian;
            float maxSlopeInPercentage;
            float maxSlopeSpeedVariation;

            float timeKeepMoveInAir; //in second
            float percentageControlInAir;

            float maxVerticalSpeed; //in units/second

            float maxWalkSpeed; //in units/second
            float maxRunSpeed; //in units/second
    };

}
