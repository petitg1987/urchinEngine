#include <stdexcept>
#include <cstring>
#include "UrchinCommon.h"

#include "player/filereader/SoundFileReader.h"

namespace urchin
{

	SoundFileReader::SoundFileReader(const std::string &filename) :
		bEndOfFileReached(false)
	{
		//open file
		std::memset (&fileInfos, 0, sizeof (fileInfos)) ;
		std::string filenamePath = FileSystem::instance()->getWorkingDirectory() + filename;
		file = sf_open(filenamePath.c_str(), SFM_READ, &fileInfos);
		if (!file)
		{
			throw std::invalid_argument("Impossible to open sound file named: " + filenamePath + ".");
		}

		//determine sound format
		if(fileInfos.channels==1)
		{
			format = AL_FORMAT_MONO16;
		}else if(fileInfos.channels==2)
		{
			format = AL_FORMAT_STEREO16;
		}else
		{
			sf_close(file);
			throw std::domain_error("Unsupported channels: " + Converter::toString(fileInfos.channels) + ".");
		}
	}

	SoundFileReader::~SoundFileReader()
	{
		sf_close(file);
	}

	/**
	 * Read next chunk of the sound file to fill buffer up to his maximum capacity
	 * @param buffer [out] Buffer to fill with samples
	 * @return Number of samples read. Could be differ from buffer capacity when end of file is reach.
	 */
	ALsizei SoundFileReader::readNextChunk(std::vector<ALshort> &buffer)
	{
		ALsizei nbSampleRead = sf_read_short(file, &buffer[0], buffer.size());
		bEndOfFileReached = buffer.size() != static_cast<unsigned int>(nbSampleRead);

		return nbSampleRead;
	}

	void SoundFileReader::resetCursor()
	{
		sf_seek(file, 0, SEEK_SET);

		bEndOfFileReached = false;
	}

	bool SoundFileReader::isEndOfFileReached() const
	{
		return bEndOfFileReached;
	}

	ALenum SoundFileReader::getFormat() const
	{
		return format;
	}

	/**
	 * @return Number of samples (frames * channels)
	 */
	ALsizei SoundFileReader::getNumberOfSamples() const
	{
		return fileInfos.channels * fileInfos.frames;
	}

	/**
	 * @return Number of channels (1=mono, 2=stereo...)
	 */
	ALsizei SoundFileReader::getNumberOfChannels() const
	{
		return fileInfos.channels;
	}

	/**
	 * @return Number of samples to read by second (frequency)
	 */
	ALsizei SoundFileReader::getSampleRate() const
	{
		return fileInfos.samplerate;
	}

	/**
	 * @return Sound duration in second
	 */
	float SoundFileReader::getSoundDuration() const
	{
		return static_cast<float>(fileInfos.frames) / static_cast<float>(fileInfos.samplerate);
	}

}
