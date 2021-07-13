#include "SoundBoxReaderWriter.h"

namespace urchin {

    std::unique_ptr<SoundShape> SoundBoxReaderWriter::loadFrom(const DataChunk* shapeChunk, const DataParser& dataParser) const {
        auto halfSizesChunk = dataParser.getUniqueChunk(true, HALF_SIZES_TAG, UdaAttribute(), shapeChunk);
        Vector3<float> halfSizes = halfSizesChunk->getVector3Value();

        auto positionChunk = dataParser.getUniqueChunk(true, POSITION_TAG, UdaAttribute(), shapeChunk);
        Point3<float> position = positionChunk->getPoint3Value();

        auto orientationChunk = dataParser.getUniqueChunk(true, ORIENTATION_TAG, UdaAttribute(), shapeChunk);
        auto orientationAxisChunk = dataParser.getUniqueChunk(true, AXIS_TAG, UdaAttribute(), orientationChunk);
        auto orientationAngleChunk = dataParser.getUniqueChunk(true, ANGLE_TAG, UdaAttribute(), orientationChunk);
        Quaternion<float> orientation(orientationAxisChunk->getVector3Value(), orientationAngleChunk->getFloatValue());

        auto marginChunk = dataParser.getUniqueChunk(true, MARGIN_TAG, UdaAttribute(), shapeChunk);
        float margin = marginChunk->getFloatValue();

        return std::make_unique<SoundBox>(halfSizes, position, orientation, margin);
    }

    void SoundBoxReaderWriter::writeOn(DataChunk& shapeChunk, const SoundShape& soundShape, UdaWriter& udaWriter) const {
        shapeChunk.addAttribute(UdaAttribute(TYPE_ATTR, BOX_VALUE));

        const auto& boxShape = dynamic_cast<const SoundBox&>(soundShape);

        auto& halfSizesChunk = udaWriter.createChunk(HALF_SIZES_TAG, UdaAttribute(), &shapeChunk);
        halfSizesChunk.setVector3Value(boxShape.getHalfSizes());

        auto& positionChunk = udaWriter.createChunk(POSITION_TAG, UdaAttribute(), &shapeChunk);
        positionChunk.setPoint3Value(boxShape.getCenterPosition());

        auto& orientationChunk = udaWriter.createChunk(ORIENTATION_TAG, UdaAttribute(), &shapeChunk);
        auto& orientationAxisChunk = udaWriter.createChunk(AXIS_TAG, UdaAttribute(), &orientationChunk);
        auto& orientationAngleChunk = udaWriter.createChunk(ANGLE_TAG, UdaAttribute(), &orientationChunk);
        Vector3<float> orientationAxis;
        float orientationAngle;
        boxShape.getOrientation().toAxisAngle(orientationAxis, orientationAngle);
        orientationAxisChunk.setVector3Value(orientationAxis);
        orientationAngleChunk.setFloatValue(orientationAngle);

        auto& marginChunk = udaWriter.createChunk(MARGIN_TAG, UdaAttribute(), &shapeChunk);
        marginChunk.setFloatValue(boxShape.getMargin());
    }

}
