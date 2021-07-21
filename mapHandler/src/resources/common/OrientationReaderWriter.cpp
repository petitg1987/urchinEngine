#include "OrientationReaderWriter.h"

namespace urchin {

    Quaternion<float> OrientationReaderWriter::loadOrientation(const UdaChunk* parentChunk, const UdaParser& udaParser) {
        auto orientationChunk = udaParser.getUniqueChunk(true, ORIENTATION_TAG, UdaAttribute(), parentChunk);

        auto orientationAxisChunk = udaParser.getUniqueChunk(true, AXIS_TAG, UdaAttribute(), orientationChunk);
        auto orientationAngleChunk = udaParser.getUniqueChunk(true, ANGLE_TAG, UdaAttribute(), orientationChunk);

        return Quaternion<float>(orientationAxisChunk->getVector3Value(), orientationAngleChunk->getFloatValue());
    }

    void OrientationReaderWriter::writeOrientation(UdaChunk& parentChunk, const Quaternion<float>& orientation, UdaWriter& udaWriter) {
        auto& orientationChunk = udaWriter.createChunk(ORIENTATION_TAG, UdaAttribute(), &parentChunk);

        auto& orientationAxisChunk = udaWriter.createChunk(AXIS_TAG, UdaAttribute(), &orientationChunk);
        auto& orientationAngleChunk = udaWriter.createChunk(ANGLE_TAG, UdaAttribute(), &orientationChunk);

        Vector3<float> orientationAxis;
        float orientationAngle;
        orientation.toAxisAngle(orientationAxis, orientationAngle);

        orientationAxisChunk.setVector3Value(orientationAxis);
        orientationAngleChunk.setFloatValue(orientationAngle);
    }

}
