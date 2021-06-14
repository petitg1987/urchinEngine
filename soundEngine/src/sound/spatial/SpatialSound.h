#pragma once

#include <string>
#include <UrchinCommon.h>

#include <sound/Sound.h>

namespace urchin {

    /**
    * Spatialized sound with waves radiate uniformly in all directions from a given location in space.
    * Sound is attenuated when listener distance increases.
    */
    class SpatialSound : public Sound {
        public:
            SpatialSound(std::string, const Point3<float>&);
            SpatialSound(std::string, const Point3<float>&, float);

            Sound::SoundType getSoundType() const override;

            void setPosition(const Point3<float>&);
            Point3<float> getPosition() const;

            void setInaudibleDistance(float);
            float getInaudibleDistance() const;

        private:
            void initializeSource();

            Point3<float> position;
            float inaudibleDistance;
    };

}
