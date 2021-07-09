#include "SoundBoxReaderWriter.h"

namespace urchin {

    SoundShape* SoundBoxReaderWriter::loadFrom(const XmlChunk* shapeChunk, const XmlParser& xmlParser) const {
        auto halfSizesChunk = xmlParser.getUniqueChunk(true, HALF_SIZES_TAG, XmlAttribute(), shapeChunk);
        Vector3<float> halfSizes = halfSizesChunk->getVector3Value();

        auto positionChunk = xmlParser.getUniqueChunk(true, POSITION_TAG, XmlAttribute(), shapeChunk);
        Point3<float> position = positionChunk->getPoint3Value();

        auto orientationChunk = xmlParser.getUniqueChunk(true, ORIENTATION_TAG, XmlAttribute(), shapeChunk);
        auto orientationAxisChunk = xmlParser.getUniqueChunk(true, AXIS_TAG, XmlAttribute(), orientationChunk.get());
        auto orientationAngleChunk = xmlParser.getUniqueChunk(true, ANGLE_TAG, XmlAttribute(), orientationChunk.get());
        Quaternion<float> orientation(orientationAxisChunk->getVector3Value(), orientationAngleChunk->getFloatValue());

        auto marginChunk = xmlParser.getUniqueChunk(true, MARGIN_TAG, XmlAttribute(), shapeChunk);
        float margin = marginChunk->getFloatValue();

        return new SoundBox(halfSizes, position, orientation, margin);
    }

    void SoundBoxReaderWriter::writeOn(XmlChunk* shapeChunk, const SoundShape* soundShape, XmlWriter& xmlWriter) const {
        shapeChunk->setAttribute(XmlAttribute(TYPE_ATTR, BOX_VALUE));

        const auto* boxShape = dynamic_cast<const SoundBox*>(soundShape);

        auto halfSizesChunk = xmlWriter.createChunk(HALF_SIZES_TAG, XmlAttribute(), shapeChunk);
        halfSizesChunk->setVector3Value(boxShape->getHalfSizes());

        auto positionChunk = xmlWriter.createChunk(POSITION_TAG, XmlAttribute(), shapeChunk);
        positionChunk->setPoint3Value(boxShape->getCenterPosition());

        auto orientationChunk = xmlWriter.createChunk(ORIENTATION_TAG, XmlAttribute(), shapeChunk);
        auto orientationAxisChunk = xmlWriter.createChunk(AXIS_TAG, XmlAttribute(), orientationChunk.get());
        auto orientationAngleChunk = xmlWriter.createChunk(ANGLE_TAG, XmlAttribute(), orientationChunk.get());
        Vector3<float> orientationAxis;
        float orientationAngle;
        boxShape->getOrientation().toAxisAngle(orientationAxis, orientationAngle);
        orientationAxisChunk->setVector3Value(orientationAxis);
        orientationAngleChunk->setFloatValue(orientationAngle);

        auto marginChunk = xmlWriter.createChunk(MARGIN_TAG, XmlAttribute(), shapeChunk);
        marginChunk->setFloatValue(boxShape->getMargin());
    }

}
