#ifndef ENGINE_LIGHTREADERWRITER_H
#define ENGINE_LIGHTREADERWRITER_H

#include <memory>
#include <string>

#include "UrchinCommon.h"
#include "Urchin3dEngine.h"

namespace urchin
{

	class LightReaderWriter
	{
		//XML attributes
		#define TYPE_ATTR "type"

		//XML value
		#define OMNIDIRECTIONAL_VALUE "OMNIDIRECTIONAL"
		#define SUN_VALUE "SUN"

		//XML tags
		#define DIRECTION_TAG "direction"
		#define POSITION_TAG "position"
		#define AMBIENT_COLOR_TAG "ambientColor"
		#define EXPENENTIAL_ATTENUATION_TAG "exponentialAttenuation"
		#define PRODUCE_SHADOW_TAG "produceShadow"

		public:
			LightReaderWriter();
			~LightReaderWriter();

			Light *loadFrom(std::shared_ptr<XmlChunk>, const XmlParser &) const;
			void writeOn(std::shared_ptr<XmlChunk>, const Light *, XmlWriter &) const;

		private:
			Light *buildLightFrom(std::shared_ptr<XmlChunk>, const XmlParser &) const;
			void buildChunkFrom(std::shared_ptr<XmlChunk>, const Light *, XmlWriter &) const;

			void loadPropertiesOn(Light *, std::shared_ptr<XmlChunk>, const XmlParser &) const;
			void writePropertiesOn(std::shared_ptr<XmlChunk>, const Light *, XmlWriter &) const;

			void loadFlagsOn(Light *, std::shared_ptr<XmlChunk>, const XmlParser &) const;
			void writeFlagsOn(std::shared_ptr<XmlChunk>, const Light *, XmlWriter &) const;
	};

}

#endif
