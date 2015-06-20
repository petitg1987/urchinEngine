#ifndef ENGINE_STREAMUPDATEWORKER_H
#define ENGINE_STREAMUPDATEWORKER_H

#include <vector>
#include <boost/thread.hpp>

#include "player/stream/StreamUpdateTask.h"

namespace urchin
{

	/**
	* Thread which refresh Open AL buffers for sounds play in streaming
	*/
	class StreamUpdateWorker
	{
		public:
			StreamUpdateWorker();
			~StreamUpdateWorker();

			void addTask(const Sound *, bool);
			void removeTask(const Sound *);
			bool isTaskExist(const Sound *) const;

			void start();
			void interrupt();

		private:
			bool continueExecution();

			bool processTask(StreamUpdateTask *);
			void deleteTask(StreamUpdateTask *);

			void fillAndPushChunk(StreamUpdateTask *, unsigned int);
			void fillChunk(StreamUpdateTask *, unsigned int);
			unsigned int retrieveChunkId(StreamUpdateTask *, ALuint);
			void clearQueue(StreamUpdateTask *);

			const unsigned int nbChunkBuffer;
			const unsigned int nbSecondByChunk;
			const unsigned int updateStreamBufferPauseTime;

			bool streamUpdateWorkerStopper;
			boost::recursive_mutex stopperMutex;
			mutable boost::recursive_mutex tasksMutex;

			std::vector<StreamUpdateTask *> tasks;
	};

}

#endif
