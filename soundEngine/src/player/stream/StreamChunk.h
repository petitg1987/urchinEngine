#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <vector>

namespace urchin {

    /**
    * Stream chunk buffer
    */
    struct StreamChunk {
        StreamChunk();

        std::vector<ALshort> samples; //samples of the chunk
        unsigned int numberOfSamples; //number of samples. Should match samples.size() except when the end of file is reached && !playLoop.

        ALuint bufferId; //OpenAL buffer ID
    };

}
