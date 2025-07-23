#include <AL/al.h>
#include <UrchinCommon.h>

#include "player/stream/StreamUpdateWorker.h"
#include "util/CheckState.h"

namespace urchin {

    StreamUpdateWorker::StreamUpdateWorker() :
            nbChunkBuffer(ConfigService::instance().getUnsignedIntValue("player.numberOfStreamBuffer")),
            chunkSizeInMs(ConfigService::instance().getUnsignedIntValue("player.streamChunkSizeInMs")),
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
        CheckState::check("generate buffers id", nbChunkBuffer);
        for (unsigned int chunkIndex = 0; chunkIndex < nbChunkBuffer; ++chunkIndex) {
            task->getStreamChunk(chunkIndex).bufferId = bufferId[chunkIndex];
        }

        //initialize buffers/chunks
        if (audioStreamPlayer.getSound().hasPreLoadedChunks()) {
            unsigned int totalSamplesRead = 0;
            for (unsigned int chunkIndex = 0; chunkIndex < nbChunkBuffer; ++chunkIndex) {
                fillChunkFromPreLoadedData(*task, chunkIndex, audioStreamPlayer.getSound().getPreLoadedChunk(chunkIndex, playLoop));
                pushChunkInQueue(*task, chunkIndex);
                totalSamplesRead += task->getStreamChunk(chunkIndex).numberOfSamples;
            }
            task->setInitialReadSamples(totalSamplesRead);
        } else {
            for (unsigned int chunkIndex = 0; chunkIndex < nbChunkBuffer; ++chunkIndex) {
                fillChunkFromFile(*task, chunkIndex);
                pushChunkInQueue(*task, chunkIndex);
            }
        }

        std::scoped_lock lock(tasksMutex);
        #ifdef URCHIN_DEBUG
            assert(!std::ranges::any_of(tasks, [&audioStreamPlayer](const auto& t) { return t->getSourceId() == audioStreamPlayer.getSourceId(); }));
        #endif
        tasks.push_back(std::move(task));
    }

    void StreamUpdateWorker::removeTask(const AudioStreamPlayer& audioStreamPlayer) {
        std::scoped_lock lock(tasksMutex);

        auto itFind = std::ranges::find_if(tasks, [&audioStreamPlayer](const auto& task){ return task->getSourceId() == audioStreamPlayer.getSourceId(); });
        if (itFind != tasks.end()) {
            deleteTask(*(*itFind));
            tasks.erase(itFind);
        }
    }

    void StreamUpdateWorker::interruptThread() {
        streamUpdateWorkerStopper.store(true, std::memory_order_release);
    }

    void StreamUpdateWorker::start() {
        try {
            Logger::instance().logInfo("Sound stream thread started");

            while (continueExecution()) {
                {
                    std::scoped_lock lock(tasksMutex);

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

                if (SleepUtil::stepSleep(updateStreamBufferPauseTime, this)) {
                    break;
                }
            }
        } catch (const std::exception& e) {
            Logger::instance().logError("Error cause sound thread crash: " + std::string(e.what()));
            //note: do not report exception to main thread because crash of sound thread is not enough important to make the application crash
        }
    }

    /**
     * @return True if thread execution is not interrupted
     */
    bool StreamUpdateWorker::continueExecution() const {
        return !streamUpdateWorkerStopper.load(std::memory_order_acquire);
    }

    bool StreamUpdateWorker::processTask(StreamUpdateTask& task) const {
        task.initializeReadCursor();

        ALint chunkProcessed = 0;
        alGetSourcei(task.getSourceId(), AL_BUFFERS_PROCESSED, &chunkProcessed);
        CheckState::check("get buffers processed (process)");

        for (int i = 0; i < chunkProcessed; ++i) {
            //pop the first unused buffer from the queue
            ALuint bufferId;
            alSourceUnqueueBuffers(task.getSourceId(), 1, &bufferId);
            CheckState::check("un-queue buffers (process)");

            unsigned int chunkIndex = retrieveChunkIndex(task, bufferId);
            fillChunkFromFile(task, chunkIndex);
            pushChunkInQueue(task, chunkIndex);
        }

        ALint nbQueues = 0;
        alGetSourcei(task.getSourceId(), AL_BUFFERS_QUEUED, &nbQueues);
        CheckState::check("get buffers queued (process)");
        return nbQueues == 0; //task terminated ?
    }

    void StreamUpdateWorker::deleteTask(StreamUpdateTask& task) const {
        #ifdef URCHIN_DEBUG
            ALint state;
            alGetSourcei(task.getSourceId(), AL_SOURCE_STATE, &state);
            assert(state == AL_INITIAL || state == AL_STOPPED);
        #endif

        clearQueue(task);
        alSourcei(task.getSourceId(), AL_BUFFER, AL_NONE);
        CheckState::check("release source buffer (worker)");

        for (unsigned int i = 0; i < nbChunkBuffer; ++i) {
            alDeleteBuffers(1, &task.getStreamChunk(i).bufferId);
            CheckState::check("delete buffers");
        }
    }

    void StreamUpdateWorker::pushChunkInQueue(StreamUpdateTask& task, unsigned int chunkIndex) const {
        const StreamChunk& streamChunk = task.getStreamChunk(chunkIndex);
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

            alBufferData(streamChunk.bufferId, format, streamChunk.samples.data(), size, (ALsizei)task.getSoundFileReader().getSampleRate());
            CheckState::check("fill buffer with audio data", size);

            alSourceQueueBuffers(task.getSourceId(), 1, &streamChunk.bufferId);
            CheckState::check("source queue buffers");
        }
    }

    void StreamUpdateWorker::fillChunkFromFile(StreamUpdateTask& task, unsigned int chunkIndex) const {
        StreamChunk& streamChunk = task.getStreamChunk(chunkIndex);
        auto chunkSize = (std::size_t)((float)task.getSoundFileReader().getSampleRate() * (float)task.getSoundFileReader().getNumberOfChannels() * ((float)chunkSizeInMs / 1000.0f));
        streamChunk.samples.resize(chunkSize);

        task.getSoundFileReader().readNextChunk(streamChunk.samples, streamChunk.numberOfSamples, task.isPlayLoop());
        streamChunk.samples.resize(streamChunk.numberOfSamples);
    }

    void StreamUpdateWorker::fillChunkFromPreLoadedData(StreamUpdateTask& task, unsigned int chunkIndex, std::vector<int16_t> preLoadedChunkData) const {
        StreamChunk& streamChunk = task.getStreamChunk(chunkIndex);
        streamChunk.samples = std::move(preLoadedChunkData);
        streamChunk.numberOfSamples = (unsigned int)streamChunk.samples.size();
    }

    unsigned int StreamUpdateWorker::retrieveChunkIndex(StreamUpdateTask& task, ALuint bufferId) const {
        for (unsigned int chunkIndex = 0; chunkIndex < nbChunkBuffer; ++chunkIndex) {
            if (task.getStreamChunk(chunkIndex).bufferId == bufferId) {
                return chunkIndex;
            }
        }

        throw std::domain_error("Stream chunk with buffer id " + std::to_string(bufferId) + " not found (" + task.getSoundFilename() + ")");
    }

    void StreamUpdateWorker::clearQueue(const StreamUpdateTask& task) const {
        ALint nbBuffersProcessed = 0;
        alGetSourcei(task.getSourceId(), AL_BUFFERS_PROCESSED, &nbBuffersProcessed);
        CheckState::check("get buffers processed (clear)");

        for (int i = 0; i < nbBuffersProcessed; ++i) {
            ALuint bufferId;
            alSourceUnqueueBuffers(task.getSourceId(), 1, &bufferId);
            CheckState::check("un-queue buffers (clear)");
        }
    }

}
