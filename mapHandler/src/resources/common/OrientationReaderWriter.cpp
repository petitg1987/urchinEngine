#include "OrientationReaderWriter.h"

namespace urchin {

    Quaternion<float> OrientationReaderWriter::loadOrientation(const DataChunk* parentChunk, const DataParser& dataParser) {
        auto orientationChunk = dataParser.getUniqueChunk(true, ORIENTATION_TAG, DataAttribute(), parentChunk);

        auto orientationAxisChunk = dataParser.getUniqueChunk(true, AXIS_TAG, DataAttribute(), orientationChunk);
        auto orientationAngleChunk = dataParser.getUniqueChunk(true, ANGLE_TAG, DataAttribute(), orientationChunk);

        return {orientationAxisChunk->getVector3Value(), orientationAngleChunk->getFloatValue()};
    }

    void OrientationReaderWriter::writeOrientation(DataChunk& parentChunk, const Quaternion<float>& orientation, DataWriter& dataWriter) {
        auto& orientationChunk = dataWriter.createChunk(ORIENTATION_TAG, DataAttribute(), &parentChunk);

        auto& orientationAxisChunk = dataWriter.createChunk(AXIS_TAG, DataAttribute(), &orientationChunk);
        auto& orientationAngleChunk = dataWriter.createChunk(ANGLE_TAG, DataAttribute(), &orientationChunk);

        Vector3<float> orientationAxis;
        float orientationAngle;
        orientation.toAxisAngle(orientationAxis, orientationAngle);

        orientationAxisChunk.setVector3Value(orientationAxis);
        orientationAngleChunk.setFloatValue(orientationAngle);
    }

}
