#include "SoundBoxReaderWriter.h"

namespace urchin {

    SoundShape* SoundBoxReaderWriter::loadFrom(const XmlChunk* shapeChunk, const DataParser& dataParser) const {
        auto halfSizesChunk = dataParser.getUniqueChunk(true, HALF_SIZES_TAG, DataAttribute(), shapeChunk);
        Vector3<float> halfSizes = halfSizesChunk->getVector3Value();

        auto positionChunk = dataParser.getUniqueChunk(true, POSITION_TAG, DataAttribute(), shapeChunk);
        Point3<float> position = positionChunk->getPoint3Value();

        auto orientationChunk = dataParser.getUniqueChunk(true, ORIENTATION_TAG, DataAttribute(), shapeChunk);
        auto orientationAxisChunk = dataParser.getUniqueChunk(true, AXIS_TAG, DataAttribute(), orientationChunk.get());
        auto orientationAngleChunk = dataParser.getUniqueChunk(true, ANGLE_TAG, DataAttribute(), orientationChunk.get());
        Quaternion<float> orientation(orientationAxisChunk->getVector3Value(), orientationAngleChunk->getFloatValue());

        auto marginChunk = dataParser.getUniqueChunk(true, MARGIN_TAG, DataAttribute(), shapeChunk);
        float margin = marginChunk->getFloatValue();

        return new SoundBox(halfSizes, position, orientation, margin);
    }

    void SoundBoxReaderWriter::writeOn(XmlChunk* shapeChunk, const SoundShape* soundShape, DataWriter& dataWriter) const {
        shapeChunk->setAttribute(DataAttribute(TYPE_ATTR, BOX_VALUE));

        const auto* boxShape = dynamic_cast<const SoundBox*>(soundShape);

        auto halfSizesChunk = dataWriter.createChunk(HALF_SIZES_TAG, DataAttribute(), shapeChunk);
        halfSizesChunk->setVector3Value(boxShape->getHalfSizes());

        auto positionChunk = dataWriter.createChunk(POSITION_TAG, DataAttribute(), shapeChunk);
        positionChunk->setPoint3Value(boxShape->getCenterPosition());

        auto orientationChunk = dataWriter.createChunk(ORIENTATION_TAG, DataAttribute(), shapeChunk);
        auto orientationAxisChunk = dataWriter.createChunk(AXIS_TAG, DataAttribute(), orientationChunk.get());
        auto orientationAngleChunk = dataWriter.createChunk(ANGLE_TAG, DataAttribute(), orientationChunk.get());
        Vector3<float> orientationAxis;
        float orientationAngle;
        boxShape->getOrientation().toAxisAngle(orientationAxis, orientationAngle);
        orientationAxisChunk->setVector3Value(orientationAxis);
        orientationAngleChunk->setFloatValue(orientationAngle);

        auto marginChunk = dataWriter.createChunk(MARGIN_TAG, DataAttribute(), shapeChunk);
        marginChunk->setFloatValue(boxShape->getMargin());
    }

}
