#include <stdexcept>
#include <cstring>
#include <UrchinCommon.h>

#include <player/filereader/SoundFileReader.h>

namespace urchin {

    SoundFileReader::SoundFileReader(std::string filename) :
            filename(std::move(filename)),
            vorbisFile({}),
            vorbisInfo(nullptr) {
        std::string filenamePath = FileSystem::instance()->getResourcesDirectory() + this->filename;
        stream.open(filenamePath, std::ios::binary);
        if (!stream) {
            throw std::invalid_argument("Impossible to open sound file " + this->filename);
        }

        const ov_callbacks callbacks = {SoundFileReader::read, SoundFileReader::seek, nullptr, SoundFileReader::tell};
        int loadStatus = ov_open_callbacks(&stream, &vorbisFile, nullptr, 0, callbacks);
        if (loadStatus < 0) {
            throw std::invalid_argument("Impossible to load sound file " + this->filename + ": " + std::to_string(loadStatus));
        }
        vorbisInfo = ov_info(&vorbisFile, -1);

        //determine sound format
        if (getNumberOfChannels() == 1) {
            format = SoundFormat::MONO_16;
        } else if (getNumberOfChannels() == 2) {
            format = SoundFormat::STEREO_16;
        } else {
            closeSoundFile();
            throw std::domain_error("Unsupported channels: " + std::to_string(getNumberOfChannels()) + ".");
        }
    }

    SoundFileReader::~SoundFileReader() {
        closeSoundFile();
    }

    /**
     * Read next chunk of the sound file to fill buffer up to his maximum capacity
     * @param buffer [out] Buffer to fill with samples
     * @param numSamplesRead [out] Number of samples read
     * @return True when all samples are read. In case of play loop, the result is always false.
     */
    void SoundFileReader::readNextChunk(std::vector<int16_t>& buffer, unsigned int& numSamplesRead, bool playLoop) const {
        numSamplesRead = 0;
        while (numSamplesRead < buffer.size()) {
            int bytesToRead = (int)(buffer.size() - numSamplesRead) * (int)sizeof(int16_t);
            int bytesRead = (int)ov_read(&vorbisFile, reinterpret_cast<char*>(&buffer[numSamplesRead]), bytesToRead, 0, 2, 1, nullptr);

            if (bytesRead > 0) {
                numSamplesRead += (unsigned int)bytesRead / (unsigned int)sizeof(int16_t);
            } else if (bytesRead == 0) { //end of file
                if (playLoop) {
                    bool resetResult = ov_time_seek(&vorbisFile, 0);
                    if (resetResult != 0) {
                        logReadChunkError("Impossible to reset cursor on sound file " + filename + ": " + std::to_string(resetResult));
                    }
                } else {
                    break;
                }
            } else {
                logReadChunkError("Impossible to read chunk from sound file " + filename + ": " + std::to_string(bytesRead));
            }
        }
    }

    std::size_t SoundFileReader::read(void* buffer, std::size_t elementSize, std::size_t elementCount, void* dataSource) {
        if(elementSize != 1) {
            throw std::runtime_error("Unsupported read from sound file with element size of: " + std::to_string(elementSize));
        }
        std::ifstream& stream = *static_cast<std::ifstream*>(dataSource);
        stream.read(static_cast<char*>(buffer), (long)elementCount);
        const std::streamsize bytesRead = stream.gcount();
        stream.clear(); //in case we read past EOF
        return (std::size_t)bytesRead;
    }

    int SoundFileReader::seek(void* dataSource, ogg_int64_t offset, int origin) {
        static const std::vector<std::ios_base::seekdir> seekDirections = {std::ios_base::beg, std::ios_base::cur, std::ios_base::end};
        std::ifstream& stream = *static_cast<std::ifstream*>(dataSource);
        stream.seekg(offset, seekDirections.at((unsigned long)origin));
        stream.clear(); //in case we seeked to EOF
        return 0;
    }

    long SoundFileReader::tell(void* dataSource) {
        std::ifstream& stream = *static_cast<std::ifstream*>(dataSource);
        const auto position = stream.tellg();
        assert(position >= 0);
        return (long)position;
    }

    void SoundFileReader::closeSoundFile() {
        if (vorbisFile.datasource) {
            ov_clear(&vorbisFile);
            vorbisFile.datasource = nullptr;
        }
        if (stream.is_open()) {
            stream.close();
        }
    }

    void SoundFileReader::logReadChunkError(const std::string& errorMessage) const {
        static bool errorLogged = false;
        if (!errorLogged) {
            Logger::instance()->logError(errorMessage);
            errorLogged = true;
        }
    }

    SoundFileReader::SoundFormat SoundFileReader::getFormat() const {
        return format;
    }

    /**
     * @return Number of samples (channels * frames)
     */
    unsigned int SoundFileReader::getNumberOfSamples() const {
        return getNumberOfChannels() * (unsigned int)ov_pcm_total(&vorbisFile, -1);
    }

    /**
     * @return Number of channels (1=mono, 2=stereo...)
     */
    unsigned int SoundFileReader::getNumberOfChannels() const {
        return (unsigned int)vorbisInfo->channels;
    }

    /**
     * @return Number of samples to read by second (frequency)
     */
    unsigned int SoundFileReader::getSampleRate() const {
        return (unsigned int)vorbisInfo->rate;
    }

    /**
     * @return Sound duration in second
     */
    float SoundFileReader::getSoundDuration() const {
        return (float)ov_pcm_total(&vorbisFile, -1) / (float)getSampleRate();
    }

}
