#pragma once

#include <memory>
#include <string>

#include <UrchinCommon.h>

namespace urchin {

    class OrientationReaderWriter {
        public:
            static Quaternion<float> loadOrientation(const UdaChunk*, const UdaParser&);
            static void writeOrientation(UdaChunk&, const Quaternion<float>&, UdaWriter&);

        private:
            static constexpr char ORIENTATION_TAG[] = "orientation";
            static constexpr char AXIS_TAG[] = "axis";
            static constexpr char ANGLE_TAG[] = "angle";
    };

}
