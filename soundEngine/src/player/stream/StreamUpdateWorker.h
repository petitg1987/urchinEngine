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
            StreamUpdateWorker();
            ~StreamUpdateWorker();

            void addTask(const Sound*, bool);
            void removeTask(const Sound*);
            bool isTaskExist(const Sound*) const;

            void start();
            void interrupt();
            void controlExecution();

        private:
            bool continueExecution();

            bool processTask(StreamUpdateTask*);
            void deleteTask(StreamUpdateTask*);

            void fillAndPushChunk(StreamUpdateTask*, unsigned int);
            void fillChunk(StreamUpdateTask*, unsigned int) const;
            unsigned int retrieveChunkId(StreamUpdateTask*, ALuint) const;
            void clearQueue(StreamUpdateTask*) const;

            const unsigned int nbChunkBuffer;
            const unsigned int nbSecondByChunk;
            const unsigned int updateStreamBufferPauseTime;

            std::atomic_bool streamUpdateWorkerStopper;
            static std::exception_ptr soundThreadExceptionPtr;
            mutable std::mutex tasksMutex;

            std::vector<StreamUpdateTask*> tasks;
    };

}
