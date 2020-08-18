#include <utility>
#include <stdexcept>
#include <cassert>
#include <AL/alc.h>

#include "sound/Sound.h"

namespace urchin
{

    Sound::Sound(std::string filename) :
        sourceId(0),
        filename(std::move(filename)),
        volume(1.0f)
    {
        if(!alcGetCurrentContext())
        {
            throw std::runtime_error("No OpenAL context found: check that a sound manager has been created");
        }

        alGenSources(1, &sourceId);
        alSourcef(sourceId, AL_GAIN, volume);
    }

    Sound::~Sound()
    {
        alSourcei(sourceId, AL_BUFFER, 0);
        alDeleteSources(1, &sourceId);
    }

    /**
     * @return Source id usable by OpenAL
     */
    ALuint Sound::getSourceId() const
    {
        return sourceId;
    }

    const std::string &Sound::getFilename() const
    {
        return filename;
    }

    /**
     * @return True if source is stopped (not playing, not in pause)
     */
    bool Sound::isStopped() const
    {
        ALint status;
        alGetSourcei(getSourceId(), AL_SOURCE_STATE, &status);

        return status==AL_STOPPED;
    }

    bool Sound::isPaused() const
    {
        ALint status;
        alGetSourcei(getSourceId(), AL_SOURCE_STATE, &status);

        return status==AL_PAUSED;
    }

    bool Sound::isPlaying() const
    {
        ALint status;
        alGetSourcei(getSourceId(), AL_SOURCE_STATE, &status);

        return status==AL_PLAYING;
    }

    /**
     * @param Volume to set (0.0: minimum volume | 1.0: original volume). Note that volume can be higher to 1.0.
     */
    void Sound::setVolume(float volume)
    {
        this->volume = volume;

        alSourcef(sourceId, AL_GAIN, this->volume);
    }

    /**
     * Defines sound volume change. Change is done relatively to the current volume value.
     */
    void Sound::setVolumeChange(float volumeChange)
    {
        this->volume = std::max(0.0f, this->volume + volumeChange);

        alSourcef(sourceId, AL_GAIN, this->volume);
    }

    /**
     * @return Return sound volume (0.0: minimum | 1.0: original source volume). Note that volume can be higher to 1.0.
     */
    float Sound::getVolume() const
    {
        return volume;
    }

}
