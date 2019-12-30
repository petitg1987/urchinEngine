#include <AL/al.h>
#include <algorithm>
#include <stdexcept>

#include "SoundManager.h"
#include "player/stream/AudioStreamPlayer.h"

namespace urchin
{

	SoundManager::SoundManager()
	{
		DeviceManager::instance();
        streamUpdateWorker = new StreamUpdateWorker();
        streamUpdateWorkerThread = new std::thread(&StreamUpdateWorker::start, streamUpdateWorker);

		alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
		alListener3f(AL_POSITION, 0.f, 0.f, 0.f);
	}

	SoundManager::~SoundManager()
	{
		for (auto audioController : audioControllers)
		{
			deleteAudioController(audioController);
		}

        streamUpdateWorker->interrupt();
        streamUpdateWorkerThread->join();
        delete streamUpdateWorkerThread;
        delete streamUpdateWorker;

		Profiler::getInstance("sound")->log();
	}

	void SoundManager::addSound(Sound *sound, SoundTrigger *soundTrigger)
	{
		if(sound && soundTrigger)
		{
			auto *audioController = new AudioController(sound, soundTrigger, streamUpdateWorker);
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

    void SoundManager::globalPause()
    {
        for(auto &audioController : audioControllers)
        {
            audioController->globalPause();
        }
    }

    void SoundManager::globalResume()
    {
        for(auto &audioController : audioControllers)
        {
            audioController->globalResume();
        }
    }

	void SoundManager::controlExecution()
	{
		streamUpdateWorker->controlExecution();
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
			{ //TODO is it logged + review _DEBUG
				throw std::runtime_error("OpenAL error: " + std::to_string(err));
			}
        #endif
	}

	void SoundManager::process()
	{
		ScopeProfiler profiler("sound", "soundMgrProc");

		process(Point3<float>(0.0, 0.0, 0.0));
	}

	void SoundManager::deleteAudioController(AudioController *audioController)
	{
		delete audioController;
	}
}
