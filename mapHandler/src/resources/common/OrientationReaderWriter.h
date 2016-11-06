#ifndef ENGINE_ORIENTATIONREADERWRITER_H
#define ENGINE_ORIENTATIONREADERWRITER_H

#include <memory>
#include <string>

#include "UrchinCommon.h"

namespace urchin
{

	class OrientationReaderWriter
	{
		//XML tags
		#define ORIENTATION_TAG "orientation"
		#define AXIS_TAG "axis"
		#define ANGLE_TAG "angle"

		public:
			OrientationReaderWriter();
			~OrientationReaderWriter();

			Quaternion<float> loadOrientation(std::shared_ptr<XmlChunk>, const XmlParser &) const;
			void writeOrientation(std::shared_ptr<XmlChunk>, const Quaternion<float> &, XmlWriter &) const;
	};

}

#endif
