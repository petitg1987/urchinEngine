#pragma once

#include <memory>
#include <string>

#include <UrchinCommon.h>

namespace urchin {

    class OrientationReaderWriter {
        //XML tags
        #define ORIENTATION_TAG "orientation"
        #define AXIS_TAG "axis"
        #define ANGLE_TAG "angle"

        public:
            static Quaternion<float> loadOrientation(const std::shared_ptr<XmlChunk>&, const XmlParser&);
            static void writeOrientation(const std::shared_ptr<XmlChunk>&, const Quaternion<float>&, XmlWriter&);
    };

}
