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
			throw std::domain_error("Unsupported channels: " + std::to_string(fileInfos.channels) + ".");
		}
	}

	SoundFileReader::~SoundFileReader()
	{
		sf_close(file);
	}

	/**
	 * Read next chunk of the sound file to fill buffer up to his maximum capacity
	 * @param buffer [out] Buffer to fill with samples
	 * @param numSamplesRead [out] Number of samples read
	 * @return True when all samples are read. In case of play loop, the result is always false.
	 */
	bool SoundFileReader::readNextChunk(std::vector<ALshort> &buffer, unsigned int &numSamplesRead, bool playLoop)
	{
		numSamplesRead = 0;
		bool bufferFilled;
		do
		{
			unsigned int itemsToRead = buffer.size()-numSamplesRead;
			numSamplesRead += sf_read_short(file, &buffer[numSamplesRead], itemsToRead);
			bool endOfFileReached = buffer.size() != numSamplesRead;

			if(endOfFileReached)
			{
				if(playLoop)
				{ //reset cursor to 0
					sf_seek(file, 0, SEEK_SET);
				}else
				{
					return true;
				}
			}

			bufferFilled = numSamplesRead == buffer.size();
		}while(!bufferFilled);

		return false;
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
