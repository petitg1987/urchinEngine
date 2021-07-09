#include "OrientationReaderWriter.h"

namespace urchin {

    Quaternion<float> OrientationReaderWriter::loadOrientation(const XmlChunk* parentChunk, const XmlParser& xmlParser) {
        auto orientationChunk = xmlParser.getUniqueChunk(true, ORIENTATION_TAG, XmlAttribute(), parentChunk);

        auto orientationAxisChunk = xmlParser.getUniqueChunk(true, AXIS_TAG, XmlAttribute(), orientationChunk.get());
        auto orientationAngleChunk = xmlParser.getUniqueChunk(true, ANGLE_TAG, XmlAttribute(), orientationChunk.get());

        return {orientationAxisChunk->getVector3Value(), orientationAngleChunk->getFloatValue()};
    }

    void OrientationReaderWriter::writeOrientation(const XmlChunk* parentChunk, const Quaternion<float>& orientation, XmlWriter& xmlWriter) {
        auto orientationChunk = xmlWriter.createChunk(ORIENTATION_TAG, XmlAttribute(), parentChunk);

        auto orientationAxisChunk = xmlWriter.createChunk(AXIS_TAG, XmlAttribute(), orientationChunk.get());
        auto orientationAngleChunk = xmlWriter.createChunk(ANGLE_TAG, XmlAttribute(), orientationChunk.get());

        Vector3<float> orientationAxis;
        float orientationAngle;
        orientation.toAxisAngle(orientationAxis, orientationAngle);

        orientationAxisChunk->setVector3Value(orientationAxis);
        orientationAngleChunk->setFloatValue(orientationAngle);
    }

}
