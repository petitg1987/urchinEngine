#pragma once

#include <string>
#include <UrchinCommon.h>

#include <sound/Sound.h>

namespace urchin {

    /**
    * Spatialized sound with waves radiate uniformly in all directions from a given location in space.
    * Sound is attenuated when listener distance increases.
    */
    class SpatialSound final : public Sound {
        public:
            SpatialSound(std::string, SoundCategory, float, const Point3<float>&, float);

            void initializeSource(ALuint) const override;
            void updateSource(ALuint) override;

            SoundType getSoundType() const override;

            void setPosition(const Point3<float>&);
            Point3<float> getPosition() const;

            void setRadius(float);
            float getRadius() const;

        private:
            Point3<float> position;
            bool positionUpdated;

            float radius;
            bool radiusUpdated;
    };

}
