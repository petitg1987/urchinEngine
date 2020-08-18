#ifndef URCHINENGINE_STREAMCHUNK_H
#define URCHINENGINE_STREAMCHUNK_H

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

        std::vector<ALshort> samples; //samples of the chunk
        unsigned int numberOfSamples; //number of samples. Match to samples.size() except when end of file is reached && !playLoop.

        ALuint bufferId; //OpenAL buffer ID
    };

}

#endif
