#ifndef URCHINENGINE_STREAMUPDATETASK_H
#define URCHINENGINE_STREAMUPDATETASK_H

#include <AL/al.h>

#include "sound/Sound.h"
#include "player/filereader/SoundFileReader.h"
#include "player/stream/StreamChunk.h"

namespace urchin
{

	/**
	* Task for the stream worker
	*/
	class StreamUpdateTask
	{
		public:
			StreamUpdateTask(const Sound *sound, StreamChunk *, bool);
			~StreamUpdateTask();

			ALuint getSourceId() const;
			bool isSourceStopped() const;
			SoundFileReader *getSoundFileReader();
			bool isPlayLoop() const;

			StreamChunk &getStreamChunk(unsigned int);
			StreamChunk *getStreamChunks();

		private:
			const Sound *sound;
			SoundFileReader *soundFileReader;
			bool playLoop;

			StreamChunk *streamChunks;
	};

}

#endif
