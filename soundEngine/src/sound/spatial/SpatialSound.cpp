#include <utility>

#include <sound/spatial/SpatialSound.h>
#include <util/CheckState.h>

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
        CheckState::check("set source relative to false");

        alSource3f(sourceId, AL_POSITION, position.X, position.Y, position.Z);
        CheckState::check("set source position (init)");

        //AL_INVERSE_DISTANCE_CLAMPED: sound volume/gain decrease more and more quickly with the distance.
        //Formula is (see https://www.openal.org/documentation/OpenAL_Programmers_Guide.pdf):
        //  distance = clamp(distance, AL_REFERENCE_DISTANCE, AL_MAX_DISTANCE);
        //  gain/volume = AL_REFERENCE_DISTANCE / (AL_REFERENCE_DISTANCE + AL_ROLLOFF_FACTOR * (distance – AL_REFERENCE_DISTANCE));
        //
        //Formula can be visualized on https://www.desmos.com/calculator :
        //  Expression: "1.0 / (1.0 + 2.0 * (min(max(x, 1.0), 15.0) – 1.0))"
        //  Values used: 1.0 = AL_REFERENCE_DISTANCE, 2.0 = AL_ROLLOFF_FACTOR, 15.0 = AL_MAX_DISTANCE
        static int soundModelInitialized = false;
        if (!soundModelInitialized) {
            alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
            CheckState::check("setup distance model");
            soundModelInitialized = true;
        }

        alSourcef(sourceId, AL_MAX_DISTANCE, inaudibleDistance);
        CheckState::check("set source max distance (init)");

        alSourcef(sourceId, AL_REFERENCE_DISTANCE, 1.0f); //no sound volume decrease over the distance from 0.0f to AL_REFERENCE_DISTANCE
        CheckState::check("set source reference distance");

        alSourcef(sourceId, AL_ROLLOFF_FACTOR, 2.0f);
        CheckState::check("set source rolloff factor");
    }

    void SpatialSound::updateSource(ALuint sourceId) {
        if (positionUpdated) {
            alSource3f(sourceId, AL_POSITION, position.X, position.Y, position.Z);
            CheckState::check("set source position (update)");
            positionUpdated = false;
        }
        if (inaudibleDistanceUpdated) {
            alSourcef(sourceId, AL_MAX_DISTANCE, inaudibleDistance);
            CheckState::check("set source max distance (update)");
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
