#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <vorbis/vorbisfile.h>

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

            explicit SoundFileReader(std::string);
            ~SoundFileReader();

            void readNextChunk(std::vector<int16_t>&, unsigned int&, bool);

            SoundFormat getFormat() const;
            unsigned int getNumberOfSamples() const;
            unsigned int getNumberOfChannels() const;
            unsigned int getSampleRate() const;
            float getSoundDuration() const;

        private:
            static std::size_t read(void*, std::size_t, std::size_t, void*);
            static int seek(void*, ogg_int64_t, int);
            static long tell(void*);

            void closeSoundFile();
            void logReadChunkError(const std::string&);

            const std::string filename;
            std::ifstream stream;
            SoundFormat format;

            mutable OggVorbis_File vorbisFile;
            vorbis_info* vorbisInfo;
    };

}
