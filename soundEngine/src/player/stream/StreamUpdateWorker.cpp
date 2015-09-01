#include <AL/al.h>
#include <AL/alc.h>
#include <boost/date_time.hpp>
#include <iostream>
#include <stdexcept>
#include "UrchinCommon.h"

#include "player/stream/StreamUpdateWorker.h"

namespace urchin
{

	StreamUpdateWorker::StreamUpdateWorker() :
		nbChunkBuffer(ConfigService::instance()->getUnsignedIntValue("player.numberOfStreamBuffer")),
		nbSecondByChunk(ConfigService::instance()->getUnsignedIntValue("player.streamChunkSizeInSecond")),
		updateStreamBufferPauseTime(ConfigService::instance()->getUnsignedIntValue("player.updateStreamBufferPauseTime"))
	{
		if(nbChunkBuffer <= 1)
		{
			throw std::domain_error("Number of chunk buffer must be greater than one.");
		}

		streamUpdateWorkerStopper.store(false, std::memory_order_relaxed);
	}

	StreamUpdateWorker::~StreamUpdateWorker()
	{
		for(unsigned int i=0; i<tasks.size(); ++i)
		{
			deleteTask(tasks[i]);
		}
	}

	/**
	 * Adds a task to the worker. Task is in charge to ensure that queue is filled.
	 * Task will be automatically removed when finished.
	 * @param sound Sound used to fill the queue
	 */
	void StreamUpdateWorker::addTask(const Sound *sound, bool playLoop)
	{
		StreamUpdateTask *task = new StreamUpdateTask(sound, new StreamChunk[nbChunkBuffer], playLoop);

		//create buffers/chunks
		ALuint bufferId[nbChunkBuffer];
		alGenBuffers(nbChunkBuffer, bufferId);
		for(unsigned int i=0; i<nbChunkBuffer; ++i)
		{
			task->getStreamChunk(i).bufferId = bufferId[i];
		}

		//initialize buffers/chunks
		for(unsigned int i=0; i<nbChunkBuffer; ++i)
		{
			if(!task->getSoundFileReader()->isEndOfFileReached())
			{
				fillAndPushChunk(task, i);
			}else if(task->isPlayLoop())
			{
				task->getSoundFileReader()->resetCursor();
				fillAndPushChunk(task, i);
			}
		}

		boost::recursive_mutex::scoped_lock lock(tasksMutex);
		#ifdef _DEBUG
			assert(!isTaskExist(sound));
		#endif

		tasks.push_back(task);
	}

	bool StreamUpdateWorker::isTaskExist(const Sound *sound) const
	{
		boost::recursive_mutex::scoped_lock lock(tasksMutex);

		for(unsigned int i=0; i<tasks.size(); ++i)
		{
			if(tasks[i]->getSourceId() == sound->getSourceId())
			{
				return true;
			}
		}

		return false;
	}

	void StreamUpdateWorker::removeTask(const Sound *sound)
	{
		boost::recursive_mutex::scoped_lock lock(tasksMutex);

		for(std::vector<StreamUpdateTask *>::iterator it=tasks.begin(); it!=tasks.end(); ++it)
		{
			if((*it)->getSourceId() == sound->getSourceId())
			{
				deleteTask(*it);
				tasks.erase(it);

				break;
			}
		}
	}

	void StreamUpdateWorker::start()
	{
		while(continueExecution())
		{
			{
				boost::recursive_mutex::scoped_lock lock(tasksMutex);

				for(std::vector<StreamUpdateTask *>::iterator it=tasks.begin(); it!=tasks.end();)
				{
					bool taskFinished = processTask(*it);
					if(taskFinished)
					{
						deleteTask(*it);
						it = tasks.erase(it);
					}else
					{
						++it;
					}
				}
			}

			boost::this_thread::sleep(boost::posix_time::milliseconds(updateStreamBufferPauseTime));
		}
	}

	/**
	 * @return True if thread execution is not interrupted
	 */
	bool StreamUpdateWorker::continueExecution()
	{
		return !streamUpdateWorkerStopper.load(std::memory_order_relaxed);
	}

	/**
	 * Interrupt the thread
	 */
	void StreamUpdateWorker::interrupt()
	{
		streamUpdateWorkerStopper.store(true, std::memory_order_relaxed);
	}

	bool StreamUpdateWorker::processTask(StreamUpdateTask *task)
	{
		ALint chunkProcessed = 0;
		alGetSourcei(task->getSourceId(), AL_BUFFERS_PROCESSED, &chunkProcessed);

		for(int i=0; i<chunkProcessed; ++i)
		{
			//pop the first unused buffer from the queue
			ALuint bufferId;
			alSourceUnqueueBuffers(task->getSourceId(), 1, &bufferId);

			unsigned int chunkId = retrieveChunkId(task, bufferId);

			if(!task->getSoundFileReader()->isEndOfFileReached())
			{
				fillAndPushChunk(task, chunkId);
			}else
			{
				if(task->isPlayLoop())
				{
					task->getSoundFileReader()->resetCursor();
					fillAndPushChunk(task, chunkId);

				}else if(task->isSourceStopped())
				{
					return true;
				}
			}
		}

		return false;
	}

	void StreamUpdateWorker::deleteTask(StreamUpdateTask *task)
	{
		clearQueue(task);
		alSourcei(task->getSourceId(), AL_BUFFER, 0);

		for(unsigned int i=0; i<nbChunkBuffer; ++i)
		{
			alDeleteBuffers(1, &task->getStreamChunk(i).bufferId);
		}

		delete [] task->getStreamChunks();
		delete task;
	}

	/**
	 * Fill chunk and push it in the queue of buffers
	 * @param task Task currently executed
	 */
	void StreamUpdateWorker::fillAndPushChunk(StreamUpdateTask *task, unsigned int chunkId)
	{
		fillChunk(task, chunkId);

		const StreamChunk &streamChunk = task->getStreamChunk(chunkId);
		ALsizei size = streamChunk.numberOfSamples * sizeof(ALushort);
		if(size > 0)
		{
			alBufferData(streamChunk.bufferId, task->getSoundFileReader()->getFormat(), &streamChunk.samples[0],
					size, task->getSoundFileReader()->getSampleRate());

			alSourceQueueBuffers(task->getSourceId(), 1, &streamChunk.bufferId);
		}
	}

	/**
	 * @param task Task currently executed
	 */
	void StreamUpdateWorker::fillChunk(StreamUpdateTask *task, unsigned int chunkId)
	{
		StreamChunk &streamChunk = task->getStreamChunk(chunkId);

		unsigned int nbSamplesToRead = task->getSoundFileReader()->getSampleRate() * task->getSoundFileReader()->getNumberOfChannels() * nbSecondByChunk;
		streamChunk.samples.resize(nbSamplesToRead);

		unsigned int nbSamplesRead = task->getSoundFileReader()->readNextChunk(streamChunk.samples);
		streamChunk.numberOfSamples = nbSamplesRead;
	}

	unsigned int StreamUpdateWorker::retrieveChunkId(StreamUpdateTask *task, ALuint bufferId)
	{
		for(unsigned int i=0; i<nbChunkBuffer; ++i)
		{
			if(task->getStreamChunk(i).bufferId == bufferId)
			{
				return i;
			}
		}

		throw std::domain_error("Stream chunk with buffer id " + Converter::toString(bufferId) + " not found.");
	}

	void StreamUpdateWorker::clearQueue(StreamUpdateTask *task)
	{
	    ALint nbQueues;
	    alGetSourcei(task->getSourceId(), AL_BUFFERS_QUEUED, &nbQueues);

	    ALuint buffer;
	    for (ALint i=0; i<nbQueues; ++i)
	    {
	        alSourceUnqueueBuffers(task->getSourceId(), 1, &buffer);
	    }
	}

}
