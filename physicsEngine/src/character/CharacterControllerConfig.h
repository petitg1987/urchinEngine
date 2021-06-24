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

            void setTimeKeepMoveInAir(float);
            float getTimeKeepMoveInAir() const;
            void setPercentageControlInAir(float);
            float getPercentageControlInAir() const;

            void setMaxVerticalSpeed(float);
            float getMaxVerticalSpeed() const;
            void setMaxHorizontalSpeed(float);
            float getMaxHorizontalSpeed() const;

        private:
            float jumpSpeed;

            float maxSlopeInRadian, maxSlopeInPercentage;

            float timeKeepMoveInAir; //in second
            float percentageControlInAir;

            float maxVerticalSpeed; //in units/second
            float maxHorizontalSpeed; // in units/second
    };

}
