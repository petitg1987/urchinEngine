#include <AL/al.h>
#include <algorithm>
#include <stdexcept>

#include "SoundManager.h"
#include "player/stream/AudioStreamPlayer.h"

namespace urchin
{

	SoundManager::SoundManager()
	{ //TODO sound doesn't work anymore in greenCity (probably because soundManager not handled by mapHandler)
		deviceManager.initializeDevice();
		AudioStreamPlayer::initializeStreamWorkerThread();

		alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
		alListener3f(AL_POSITION, 0.f, 0.f, 0.f);
	}

	SoundManager::~SoundManager()
	{
		for (auto audioController : audioControllers)
		{
			deleteAudioController(audioController);
		}

		AudioStreamPlayer::destroyStreamWorkerThread();

		deviceManager.shutdownDevice();
	}

	void SoundManager::addSound(Sound *sound, SoundTrigger *soundTrigger)
	{
		if(sound!=nullptr && soundTrigger!=nullptr)
		{
			auto *audioController = new AudioController(sound, soundTrigger);
			audioControllers.push_back(audioController);
		}
	}

	void SoundManager::removeSound(const Sound *sound)
	{
		for(auto it = audioControllers.begin(); it!=audioControllers.end(); ++it)
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
		for (auto &audioController : audioControllers)
		{
			if(audioController->getSound() == sound)
			{
				audioController->changeSoundTrigger(newSoundTrigger);

				break;
			}
		}
	}

	std::vector<const SoundTrigger *> SoundManager::getSoundTriggers() const
	{
		std::vector<const SoundTrigger *> triggers;

		for (const auto &audioController : audioControllers)
		{
			triggers.push_back(audioController->getSoundTrigger());
		}

		return triggers;
	}

	SoundTrigger *SoundManager::retrieveSoundTriggerFor(const Sound *sound) const
	{
		for (const auto &audioController : audioControllers)
		{
			if(audioController->getSound() == sound)
			{
				return audioController->getSoundTrigger();
			}
		}

		throw std::invalid_argument("Impossible to find a sound trigger for the sound.");
	}

	void SoundManager::process(const Point3<float> &listenerPosition)
	{
		alListener3f(AL_POSITION, listenerPosition.X, listenerPosition.Y, listenerPosition.Z);

		for (auto &audioController : audioControllers)
		{
			audioController->process(listenerPosition);
		}

		#ifdef _DEBUG
			ALenum err;
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
