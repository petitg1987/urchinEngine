#pragma once

#include <vector>
#include <atomic>
#include <mutex>

#include "player/stream/StreamUpdateTask.h"

namespace urchin {

    /**
    * Thread which refresh Open AL buffers for sounds play in streaming
    */
    class StreamUpdateWorker {
        public:
            friend class SleepUtil;

            StreamUpdateWorker();
            ~StreamUpdateWorker();

            void addTask(const AudioStreamPlayer&, bool);
            void removeTask(const AudioStreamPlayer&);

            void start();
            void interruptThread();

        private:
            bool continueExecution() const;

            bool processTask(StreamUpdateTask&) const;
            void deleteTask(StreamUpdateTask&) const;

            void fillChunkFromFile(StreamUpdateTask&, unsigned int) const;
            void fillChunkFromPreLoadedData(StreamUpdateTask&, unsigned int, std::vector<int16_t>) const;
            void pushChunkInQueue(StreamUpdateTask&, unsigned int) const;
            unsigned int retrieveChunkIndex(StreamUpdateTask&, ALuint) const;
            void clearQueue(const StreamUpdateTask&) const;

            const unsigned int nbChunkBuffer;
            const unsigned int chunkSizeInMs;
            const int updateStreamBufferPauseTime;

            std::atomic_bool streamUpdateWorkerStopper;
            mutable std::mutex tasksMutex;

            std::vector<std::unique_ptr<StreamUpdateTask>> tasks;
    };

}
