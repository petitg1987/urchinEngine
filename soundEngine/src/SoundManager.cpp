#include <AL/al.h>
#include <AL/alc.h>
#include <algorithm>
#include <stdexcept>

#include "SoundManager.h"
#include "player/stream/AudioStreamPlayer.h"
#include "trigger/ShapeTrigger.h"
#include "trigger/shape/SoundSphere.h"

namespace urchin
{

	SoundManager::SoundManager()
	{
		deviceManager.initializeDevice();
		AudioStreamPlayer::initializeStreamWorkerThread();

		alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
		alListener3f(AL_POSITION, 0.f, 0.f, 0.f);
	}

	SoundManager::~SoundManager()
	{
		AudioStreamPlayer::destroyStreamWorkerThread();

		for(unsigned int i=0; i<audioControllers.size(); ++i)
		{
			AudioController *audioController  = audioControllers[i];
			deleteAudioController(audioController);
		}

		deviceManager.shutdownDevice();
	}

	void SoundManager::addSound(Sound *sound, SoundTrigger *soundTrigger)
	{
		if(sound!=nullptr && soundTrigger!=nullptr)
		{
			AudioController *audioController = new AudioController(sound, soundTrigger);
			audioControllers.push_back(audioController);
		}
	}

	void SoundManager::removeSound(const Sound *sound)
	{
		for(std::vector<AudioController *>::iterator it = audioControllers.begin(); it!=audioControllers.end(); ++it)
		{
			if((*it)->getSound() == sound)
			{
				deleteAudioController(*it);
				audioControllers.erase(it);

				break;
			}
		}
	}

	void SoundManager::changeSoundTrigger(const Sound *sound, SoundTrigger *newSoundTrigger)
	{
		for(std::vector<AudioController *>::iterator it = audioControllers.begin(); it!=audioControllers.end(); ++it)
		{
			if((*it)->getSound() == sound)
			{
				(*it)->changeSoundTrigger(newSoundTrigger);

				break;
			}
		}
	}

	std::vector<const SoundTrigger *> SoundManager::getSoundTriggers() const
	{
		std::vector<const SoundTrigger *> triggers;

		for(std::vector<AudioController *>::const_iterator it=audioControllers.begin(); it!=audioControllers.end(); ++it)
		{
			triggers.push_back((*it)->getSoundTrigger());
		}

		return triggers;
	}

	SoundTrigger *SoundManager::retrieveSoundTriggerFor(const Sound *sound) const
	{
		for(std::vector<AudioController *>::const_iterator it = audioControllers.begin(); it!=audioControllers.end(); ++it)
		{
			if((*it)->getSound() == sound)
			{
				return (*it)->getSoundTrigger();
			}
		}

		throw std::invalid_argument("Impossible to find a sound trigger for the sound.");
	}

	void SoundManager::process(const Point3<float> &listenerPosition)
	{
		alListener3f(AL_POSITION, listenerPosition.X, listenerPosition.Y, listenerPosition.Z);

		for(unsigned int i=0; i<audioControllers.size(); ++i)
		{
			audioControllers[i]->process(listenerPosition);
		}

		#ifdef _DEBUG
			ALenum err = AL_NO_ERROR;
			while((err = alGetError()) != AL_NO_ERROR)
			{
				std::cout<<"OpenAL error: "<<err<<std::endl;
			}
		#endif
	}

	void SoundManager::process()
	{
		process(Point3<float>(0.0, 0.0, 0.0));
	}

	void SoundManager::deleteAudioController(AudioController *audioController)
	{
		delete audioController;
	}
}
