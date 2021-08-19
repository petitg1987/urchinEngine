#include <utility>

#include <sound/spatial/SpatialSound.h>

namespace urchin {

    /**
     * @param inaudibleDistance Define distance at which the sound become inaudible
     */
    SpatialSound::SpatialSound(std::string filename, SoundCategory category, float initialVolume, const Point3<float>& position, float inaudibleDistance) :
            Sound(std::move(filename), category, initialVolume),
            position(position),
            positionUpdated(false),
            inaudibleDistance(inaudibleDistance),
            inaudibleDistanceUpdated(false) {

    }

    void SpatialSound::initializeSource(ALuint sourceId) const {
        alSourcei(sourceId, AL_SOURCE_RELATIVE, false);
        alSource3f(sourceId, AL_POSITION, position.X, position.Y, position.Z);

        alSourcef(sourceId, AL_MAX_DISTANCE, inaudibleDistance);
        alSourcef(sourceId, AL_REFERENCE_DISTANCE, 0.0);
        alSourcef(sourceId, AL_ROLLOFF_FACTOR, 1.0);
    }

    void SpatialSound::updateSource(ALuint sourceId) {
        if (positionUpdated) {
            alSource3f(sourceId, AL_POSITION, position.X, position.Y, position.Z);
            positionUpdated = false;
        }
        if (inaudibleDistanceUpdated) {
            alSourcef(sourceId, AL_MAX_DISTANCE, inaudibleDistance);
            inaudibleDistanceUpdated = false;
        }
    }

    Sound::SoundType SpatialSound::getSoundType() const {
        return Sound::SPATIAL;
    }

    void SpatialSound::setPosition(const Point3<float>& position) {
        this->position = position;
        this->positionUpdated = true;
    }

    Point3<float> SpatialSound::getPosition() const {
        return position;
    }

    void SpatialSound::setInaudibleDistance(float inaudibleDistance) {
        this->inaudibleDistance = inaudibleDistance;
        this->inaudibleDistanceUpdated = true;
    }

    /**
     * @return Distance at which the sound become inaudible
     */
    float SpatialSound::getInaudibleDistance() const {
        return inaudibleDistance;
    }

}
