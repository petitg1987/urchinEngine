#ifndef ENGINE_STREAMCHUNK_H
#define ENGINE_STREAMCHUNK_H

#include <AL/al.h>
#include <AL/alc.h>
#include <vector>

namespace urchin
{

	/**
	* Stream chunk buffer
	*/
	struct StreamChunk
	{
		StreamChunk();
		~StreamChunk();

		 std::vector<ALshort> samples; //samples of the chunk
		 ALsizei numberOfSamples; //number of samples. Most of times it match to samples.size() except when end of file is reached.

		 ALuint bufferId; //OpenAL buffer ID
	};

}

#endif
