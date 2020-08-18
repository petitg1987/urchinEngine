#include <AL/al.h>
#include <thread>
#include <iostream>
#include "UrchinCommon.h"

#include "player/stream/StreamUpdateWorker.h"

namespace urchin
{

    //static
    std::exception_ptr StreamUpdateWorker::soundThreadExceptionPtr = nullptr;

    StreamUpdateWorker::StreamUpdateWorker() :
        nbChunkBuffer(ConfigService::instance()->getUnsignedIntValue("player.numberOfStreamBuffer")),
        nbSecondByChunk(ConfigService::instance()->getUnsignedIntValue("player.streamChunkSizeInSecond")),
        updateStreamBufferPauseTime(ConfigService::instance()->getUnsignedIntValue("player.updateStreamBufferPauseTime")),
        streamUpdateWorkerStopper(false)
    {
        if(nbChunkBuffer <= 1)
        {
            throw std::domain_error("Number of chunk buffer must be greater than one.");
        }
    }

    StreamUpdateWorker::~StreamUpdateWorker()
    {
        for (auto &task : tasks)
        {
            deleteTask(task);
        }
    }

    /**
     * Adds a task to the worker. Task is in charge to ensure that queue is filled.
     * Task will be automatically removed when finished.
     * @param sound Sound used to fill the queue
     */
    void StreamUpdateWorker::addTask(const Sound *sound, bool playLoop)
    {
        auto *task = new StreamUpdateTask(sound, new StreamChunk[nbChunkBuffer], playLoop);

        //create buffers/chunks
        ALuint *bufferId = new ALuint[nbChunkBuffer];
        alGenBuffers(nbChunkBuffer, bufferId);
        for(unsigned int i=0; i<nbChunkBuffer; ++i)
        {
            task->getStreamChunk(i).bufferId = bufferId[i];
        }
        delete []bufferId;

        //initialize buffers/chunks
        for(unsigned int i=0; i<nbChunkBuffer; ++i)
        {
            fillAndPushChunk(task, i);
        }

        #ifndef NDEBUG
            assert(!isTaskExist(sound));
        #endif

        std::lock_guard<std::mutex> lock(tasksMutex);
        tasks.push_back(task);
    }

    bool StreamUpdateWorker::isTaskExist(const Sound *sound) const
    {
        std::lock_guard<std::mutex> lock(tasksMutex);

        for (auto task : tasks)
        {
            if(task->getSourceId() == sound->getSourceId())
            {
                return true;
            }
        }

        return false;
    }

    void StreamUpdateWorker::removeTask(const Sound *sound)
    {
        std::lock_guard<std::mutex> lock(tasksMutex);

        for(auto it=tasks.begin(); it!=tasks.end(); ++it)
        {
            if((*it)->getSourceId() == sound->getSourceId())
            {
                deleteTask(*it);
                tasks.erase(it);

                break;
            }
        }
    }

    /**
      * Interrupt the thread
      */
    void StreamUpdateWorker::interrupt()
    {
        streamUpdateWorkerStopper.store(true, std::memory_order_relaxed);
    }

    void StreamUpdateWorker::controlExecution()
    {
        if(soundThreadExceptionPtr)
        {
            std::rethrow_exception(soundThreadExceptionPtr);
        }
    }

    void StreamUpdateWorker::start()
    {
        try
        {
            while (continueExecution())
            {
                {
                    std::lock_guard<std::mutex> lock(tasksMutex);

                    for (auto it = tasks.begin(); it != tasks.end();)
                    {
                        bool taskFinished = processTask(*it);
                        if (taskFinished)
                        {
                            deleteTask(*it);
                            it = tasks.erase(it);
                        } else
                        {
                            ++it;
                        }
                    }
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(updateStreamBufferPauseTime));
            }
        }catch(std::exception &e)
        {
            Logger::logger().logError("Error cause sound thread crash: exception reported to main thread");
            soundThreadExceptionPtr = std::current_exception();
        }
    }

    /**
     * @return True if thread execution is not interrupted
     */
    bool StreamUpdateWorker::continueExecution()
    {
        return !streamUpdateWorkerStopper.load(std::memory_order_relaxed);
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
            fillAndPushChunk(task, chunkId);
        }

        ALint nbQueues = 0;
        alGetSourcei(task->getSourceId(), AL_BUFFERS_QUEUED, &nbQueues);
        return nbQueues==0; //task terminated ?
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
        unsigned int bufferSize = task->getSoundFileReader()->getSampleRate() * task->getSoundFileReader()->getNumberOfChannels() * nbSecondByChunk;
        streamChunk.samples.resize(bufferSize);

        task->getSoundFileReader()->readNextChunk(streamChunk.samples, streamChunk.numberOfSamples, task->isPlayLoop());
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

        throw std::domain_error("Stream chunk with buffer id " + std::to_string(bufferId) + " not found (" + task->getSoundFilename() + ")");
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
