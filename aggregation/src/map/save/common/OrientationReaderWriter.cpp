#include "map/save/common/OrientationReaderWriter.h"

namespace urchin {

    Quaternion<float> OrientationReaderWriter::load(const UdaChunk* parentChunk, const UdaParser& udaParser) {
        auto orientationChunk = udaParser.getFirstChunk(true, ORIENTATION_TAG, UdaAttribute(), parentChunk);

        auto orientationAxisChunk = udaParser.getFirstChunk(true, AXIS_TAG, UdaAttribute(), orientationChunk);
        auto orientationAngleChunk = udaParser.getFirstChunk(true, ANGLE_TAG, UdaAttribute(), orientationChunk);

        return Quaternion<float>::fromAxisAngle(orientationAxisChunk->getVector3Value(), orientationAngleChunk->getFloatValue());
    }

    void OrientationReaderWriter::write(UdaChunk& parentChunk, const Quaternion<float>& orientation, UdaParser& udaParser) {
        auto& orientationChunk = udaParser.createChunk(ORIENTATION_TAG, UdaAttribute(), &parentChunk);

        auto& orientationAxisChunk = udaParser.createChunk(AXIS_TAG, UdaAttribute(), &orientationChunk);
        auto& orientationAngleChunk = udaParser.createChunk(ANGLE_TAG, UdaAttribute(), &orientationChunk);

        Vector3<float> orientationAxis;
        float orientationAngle;
        orientation.toAxisAngle(orientationAxis, orientationAngle);

        orientationAxisChunk.setVector3Value(orientationAxis);
        orientationAngleChunk.setFloatValue(orientationAngle);
    }

}
