#pragma once

#include <vector>
#include <atomic>
#include <mutex>

#include <player/stream/StreamUpdateTask.h>

namespace urchin {

    /**
    * Thread which refresh Open AL buffers for sounds play in streaming
    */
    class StreamUpdateWorker {
        public:
            friend class StepSleep;

            StreamUpdateWorker();
            ~StreamUpdateWorker();

            void addTask(const AudioStreamPlayer&, bool);
            void removeTask(const AudioStreamPlayer&);
            bool isTaskExist(const AudioStreamPlayer&) const;

            void start();
            void interrupt();
            void checkNoExceptionRaised() const;

        private:
            bool continueExecution() const;

            bool processTask(StreamUpdateTask&) const;
            void deleteTask(StreamUpdateTask&) const;

            void fillAndPushChunk(StreamUpdateTask&, unsigned int) const;
            void fillChunk(StreamUpdateTask&, unsigned int) const;
            unsigned int retrieveChunkId(StreamUpdateTask&, ALuint) const;
            void clearQueue(const StreamUpdateTask&) const;

            const unsigned int nbChunkBuffer;
            const unsigned int nbSecondByChunk;
            const int updateStreamBufferPauseTime;

            std::atomic_bool streamUpdateWorkerStopper;
            static std::exception_ptr soundThreadExceptionPtr;
            mutable std::mutex tasksMutex;

            std::vector<std::unique_ptr<StreamUpdateTask>> tasks;
    };

}
