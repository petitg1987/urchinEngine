#ifndef ENGINE_SOUNDFILEREADER_H
#define ENGINE_SOUNDFILEREADER_H

#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>
#include <string>
#include <vector>

namespace urchin
{

	/**
	* Allow to read a sound file to get stream data and sound information
	*/
	class SoundFileReader
	{
		public:
			SoundFileReader(const std::string &);
			~SoundFileReader();

			void readNextChunk(std::vector<ALshort> &, unsigned int &, bool);

			ALenum getFormat() const;
			ALsizei getNumberOfSamples() const;
			ALsizei getNumberOfChannels() const;
			ALsizei getSampleRate() const;
			float getSoundDuration() const;

		private:
			SNDFILE *file;
			bool bEndOfFileReached;

			SF_INFO fileInfos;
			ALenum format;
	};

}

#endif
