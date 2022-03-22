#include <AL/al.h>
#include <thread>
#include <iostream>
#include <UrchinCommon.h>

#include <player/stream/StreamUpdateWorker.h>
#include <util/CheckState.h>

namespace urchin {

    //static
    std::exception_ptr StreamUpdateWorker::soundThreadExceptionPtr = nullptr;

    StreamUpdateWorker::StreamUpdateWorker() :
            nbChunkBuffer(ConfigService::instance().getUnsignedIntValue("player.numberOfStreamBuffer")),
            nbSecondByChunk(ConfigService::instance().getUnsignedIntValue("player.streamChunkSizeInSecond")),
            updateStreamBufferPauseTime((int)ConfigService::instance().getUnsignedIntValue("player.updateStreamBufferPauseTime")),
            streamUpdateWorkerStopper(false) {
        if (nbChunkBuffer <= 1) {
            throw std::domain_error("Number of chunk buffer must be greater than one.");
        }
    }

    StreamUpdateWorker::~StreamUpdateWorker() {
        for (const auto& task : tasks) {
            deleteTask(*task);
        }
        tasks.clear();
    }

    /**
     * Adds a task to the worker. Task is in charge to ensure that queue is filled.
     * Task will be automatically removed when finished.
     * @param audioStreamPlayer Audio player used to fill the queue
     */
    void StreamUpdateWorker::addTask(const AudioStreamPlayer& audioStreamPlayer, bool playLoop) {
        ScopeProfiler sp(Profiler::sound(), "addTask");

        if (audioStreamPlayer.getSourceId() == 0) {
            return; //invalid source: probably too many sources in progress
        }
        auto task = std::make_unique<StreamUpdateTask>(audioStreamPlayer, nbChunkBuffer, playLoop);

        //create buffers/chunks
        std::vector<ALuint> bufferId(nbChunkBuffer);
        alGenBuffers((int)nbChunkBuffer, bufferId.data());
        CheckState::check("generate buffers");
        for (unsigned int i = 0; i < nbChunkBuffer; ++i) {
            task->getStreamChunk(i).bufferId = bufferId[i];
        }

        //initialize buffers/chunks
        for (unsigned int i = 0; i < nbChunkBuffer; ++i) {
            fillAndPushChunk(*task, i);
        }

        #ifdef URCHIN_DEBUG
            assert(!isTaskExist(audioStreamPlayer));
        #endif

        std::scoped_lock<std::mutex> lock(tasksMutex);
        tasks.push_back(std::move(task));
    }

    bool StreamUpdateWorker::isTaskExist(const AudioStreamPlayer& audioStreamPlayer) const {
        std::scoped_lock<std::mutex> lock(tasksMutex);

        return std::ranges::any_of(tasks, [&audioStreamPlayer](const auto& task) {
            return task->getSourceId() == audioStreamPlayer.getSourceId();
        });
    }

    void StreamUpdateWorker::removeTask(const AudioStreamPlayer& audioStreamPlayer) {
        std::scoped_lock<std::mutex> lock(tasksMutex);

        for (auto it = tasks.begin(); it != tasks.end(); ++it) {
            if ((*it)->getSourceId() == audioStreamPlayer.getSourceId()) {
                deleteTask(*(*it));
                tasks.erase(it);

                break;
            }
        }
    }

    /**
     * Interrupt the thread
     */
    void StreamUpdateWorker::interrupt() {
        streamUpdateWorkerStopper.store(true, std::memory_order_release);
    }

    void StreamUpdateWorker::checkNoExceptionRaised() const {
        if (soundThreadExceptionPtr) {
            std::rethrow_exception(soundThreadExceptionPtr);
        }
    }

    void StreamUpdateWorker::start() {
        try {
            while (continueExecution()) {
                {
                    std::scoped_lock<std::mutex> lock(tasksMutex);

                    for (auto it = tasks.begin(); it != tasks.end();) {
                        bool taskFinished = processTask(*(*it));
                        if (taskFinished) {
                            deleteTask(*(*it));
                            it = tasks.erase(it);
                        } else {
                            ++it;
                        }
                    }
                }

                if (StepSleep::sleep(updateStreamBufferPauseTime, this)) {
                    break;
                }
            }
        } catch (const std::exception&) {
            Logger::instance().logError("Error cause sound thread crash: exception reported to main thread");
            soundThreadExceptionPtr = std::current_exception();
        }
    }

    /**
     * @return True if thread execution is not interrupted
     */
    bool StreamUpdateWorker::continueExecution() const {
        return !streamUpdateWorkerStopper.load(std::memory_order_acquire);
    }

    bool StreamUpdateWorker::processTask(StreamUpdateTask& task) {
        ALint chunkProcessed = 0;
        alGetSourcei(task.getSourceId(), AL_BUFFERS_PROCESSED, &chunkProcessed);
        CheckState::check("get source buffers processed");

        for (int i = 0; i < chunkProcessed; ++i) {
            //pop the first unused buffer from the queue
            ALuint bufferId;
            alSourceUnqueueBuffers(task.getSourceId(), 1, &bufferId);
            CheckState::check("source un-queue buffers (process)");

            unsigned int chunkId = retrieveChunkId(task, bufferId);
            fillAndPushChunk(task, chunkId);
        }

        ALint nbQueues = 0;
        alGetSourcei(task.getSourceId(), AL_BUFFERS_QUEUED, &nbQueues);
        CheckState::check("get source buffers queued (process)");
        return nbQueues == 0; //task terminated ?
    }

    void StreamUpdateWorker::deleteTask(StreamUpdateTask& task) const {
        clearQueue(task);
        alSourcei(task.getSourceId(), AL_BUFFER, 0);
        CheckState::check("set source buffer to 0");

        for (unsigned int i = 0; i < nbChunkBuffer; ++i) {
            alDeleteBuffers(1, &task.getStreamChunk(i).bufferId);
            CheckState::check("delete buffers");
        }
    }

    /**
     * Fill chunk and push it in the queue of buffers
     * @param task Task currently executed
     */
    void StreamUpdateWorker::fillAndPushChunk(StreamUpdateTask& task, unsigned int chunkId) const {
        fillChunk(task, chunkId);

        const StreamChunk& streamChunk = task.getStreamChunk(chunkId);
        auto size = static_cast<ALsizei>(streamChunk.numberOfSamples * sizeof(ALushort));
        if (size > 0) {
            SoundFileReader::SoundFormat soundFormat = task.getSoundFileReader().getFormat();
            ALenum format;
            if (SoundFileReader::MONO_16 == soundFormat) {
                format = AL_FORMAT_MONO16;
            } else if (SoundFileReader::STEREO_16 == soundFormat) {
                format = AL_FORMAT_STEREO16;
            } else {
                throw std::runtime_error("Unknown sound format: " + std::to_string(task.getSoundFileReader().getFormat()));
            }

            alBufferData(streamChunk.bufferId, format, &streamChunk.samples[0], size, (ALsizei)task.getSoundFileReader().getSampleRate());
            CheckState::check("fill buffer with audio data");

            alSourceQueueBuffers(task.getSourceId(), 1, &streamChunk.bufferId);
            CheckState::check("source queue buffers");
        }
    }

    /**
     * @param task Task currently executed
     */
    void StreamUpdateWorker::fillChunk(StreamUpdateTask& task, unsigned int chunkId) const {
        StreamChunk& streamChunk = task.getStreamChunk(chunkId);
        unsigned int bufferSize = task.getSoundFileReader().getSampleRate() * task.getSoundFileReader().getNumberOfChannels() * nbSecondByChunk;
        streamChunk.samples.resize(bufferSize);

        task.getSoundFileReader().readNextChunk(streamChunk.samples, streamChunk.numberOfSamples, task.isPlayLoop());
    }

    unsigned int StreamUpdateWorker::retrieveChunkId(StreamUpdateTask& task, ALuint bufferId) const {
        for (unsigned int i = 0; i < nbChunkBuffer; ++i) {
            if (task.getStreamChunk(i).bufferId == bufferId) {
                return i;
            }
        }

        throw std::domain_error("Stream chunk with buffer id " + std::to_string(bufferId) + " not found (" + task.getSoundFilename() + ")");
    }

    void StreamUpdateWorker::clearQueue(const StreamUpdateTask& task) const {
        ALint nbQueues;
        alGetSourcei(task.getSourceId(), AL_BUFFERS_QUEUED, &nbQueues);
        CheckState::check("get source buffers queued (clean)");

        ALuint buffer;
        for (ALint i = 0; i < nbQueues; ++i) {
            alSourceUnqueueBuffers(task.getSourceId(), 1, &buffer);
            CheckState::check("source un-queue buffers (clean)");
        }
    }

}
