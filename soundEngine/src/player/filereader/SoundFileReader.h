#pragma once

#include <sndfile.h>
#include <string>
#include <vector>

namespace urchin {

    /**
    * Allow to read a sound file to get stream data and sound information
    */
    class SoundFileReader {
        public:
            enum SoundFormat {
                MONO_16,
                STEREO_16
            };

            explicit SoundFileReader(const std::string&);
            ~SoundFileReader();

            void readNextChunk(std::vector<short>&, unsigned int&, bool);

            SoundFormat getFormat() const;
            unsigned int getNumberOfSamples() const;
            unsigned int getNumberOfChannels() const;
            unsigned int getSampleRate() const;
            float getSoundDuration() const;

        private:
            SNDFILE* file;
            bool bEndOfFileReached;

            SF_INFO fileInfos;
            SoundFormat format;
    };

}
