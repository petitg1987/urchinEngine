#include <AL/al.h>
#include <AL/alc.h>
#include <stdexcept>

#include "player/stream/AudioStreamPlayer.h"

namespace urchin
{

	//static
	StreamUpdateWorker *AudioStreamPlayer::streamUpdateWorker = nullptr;
	std::thread *AudioStreamPlayer::streamUpdateWorkerThread = nullptr;

	AudioStreamPlayer::AudioStreamPlayer(const Sound *sound) :
		AudioPlayer(sound)
	{
		if(streamUpdateWorker==nullptr || streamUpdateWorkerThread==nullptr)
		{
			throw std::runtime_error("Stream update worker thread must be initialized before instantiate a stream player");
		}
	};

	AudioStreamPlayer::~AudioStreamPlayer()
	{

	}

	void AudioStreamPlayer::initializeStreamWorkerThread()
	{
		streamUpdateWorker = new StreamUpdateWorker();
		streamUpdateWorkerThread = new std::thread(&StreamUpdateWorker::start, streamUpdateWorker);
	}

	void AudioStreamPlayer::destroyStreamWorkerThread()
	{
		streamUpdateWorker->interrupt();
		streamUpdateWorkerThread->join();

		delete streamUpdateWorkerThread;
		delete streamUpdateWorker;
	}

	void AudioStreamPlayer::play()
	{
		play(false);
	}

	void AudioStreamPlayer::playLoop()
	{
		play(true);
	}

	void AudioStreamPlayer::pause()
	{
		alSourcePause(getSound()->getSourceId());
	}

	void AudioStreamPlayer::stop()
	{
		alSourceStop(getSound()->getSourceId());

		streamUpdateWorker->removeTask(getSound());
	}

	void AudioStreamPlayer::play(bool playLoop)
	{
		if(!streamUpdateWorker->isTaskExist(getSound()))
		{
			streamUpdateWorker->addTask(getSound(), playLoop);
		}

		alSourcePlay(getSound()->getSourceId());
	}

}
