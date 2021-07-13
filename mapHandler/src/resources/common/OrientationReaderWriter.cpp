#include "OrientationReaderWriter.h"

namespace urchin {

    Quaternion<float> OrientationReaderWriter::loadOrientation(const DataChunk* parentChunk, const DataParser& dataParser) {
        auto orientationChunk = dataParser.getUniqueChunk(true, ORIENTATION_TAG, UdaAttribute(), parentChunk);

        auto orientationAxisChunk = dataParser.getUniqueChunk(true, AXIS_TAG, UdaAttribute(), orientationChunk);
        auto orientationAngleChunk = dataParser.getUniqueChunk(true, ANGLE_TAG, UdaAttribute(), orientationChunk);

        return {orientationAxisChunk->getVector3Value(), orientationAngleChunk->getFloatValue()};
    }

    void OrientationReaderWriter::writeOrientation(DataChunk& parentChunk, const Quaternion<float>& orientation, UdaWriter& udaWriter) {
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
