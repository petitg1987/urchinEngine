#ifndef URCHINENGINE_SOUNDSHAPEREADERWRITERRETRIEVER_H
#define URCHINENGINE_SOUNDSHAPEREADERWRITERRETRIEVER_H

#include <memory>

#include "UrchinSoundEngine.h"
#include "resources/sound/soundshape/SoundShapeReaderWriter.h"

namespace urchin
{

	/**
	* Retrieve strategy to read/write sound shape
	*/
	class SoundShapeReaderWriterRetriever
	{
		public:
			static std::shared_ptr<SoundShapeReaderWriter> retrieveShapeReaderWriter(const std::shared_ptr<XmlChunk> &);
			static std::shared_ptr<SoundShapeReaderWriter> retrieveShapeReaderWriter(const SoundShape *);
	};

}

#endif
