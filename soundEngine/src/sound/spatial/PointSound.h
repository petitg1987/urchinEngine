#ifndef URCHINENGINE_POINTSOUND_H
#define URCHINENGINE_POINTSOUND_H

#include <string>
#include "UrchinCommon.h"

#include "sound/Sound.h"

namespace urchin {

    /**
    * Spatially located sound source whose waves radiate uniformly in all directions from a given location in space.
    * Sound is attenuated when listener distance increases.
    */
    class PointSound : public Sound {
        public:
            PointSound(const std::string&, const Point3<float>&);
            PointSound(const std::string&, const Point3<float>&, float);

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

#endif
