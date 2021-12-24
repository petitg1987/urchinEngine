#pragma once

namespace urchin {

    class CharacterControllerConfig {
        public:
            CharacterControllerConfig();

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

            void setWalkSpeed(float);
            float getWalkSpeed() const;

            void setRunSpeed(float);
            float getRunSpeed() const;

        private:
            float jumpSpeed;

            float maxSlopeInRadian, maxSlopeInPercentage;
            float maxSlopeSpeedVariation;

            float timeKeepMoveInAir; //in second
            float percentageControlInAir;

            float maxVerticalSpeed; //in units/second

            float walkSpeed; //in units/second
            float runSpeed; //in units/second
    };

}
