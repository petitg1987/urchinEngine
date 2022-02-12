#include <map/save/sound/soundshape/SoundBoxReaderWriter.h>

namespace urchin {

    std::unique_ptr<SoundShape> SoundBoxReaderWriter::load(const UdaChunk* shapeChunk, const UdaParser& udaParser) const {
        auto halfSizesChunk = udaParser.getFirstChunk(true, HALF_SIZES_TAG, UdaAttribute(), shapeChunk);
        Vector3<float> halfSizes = halfSizesChunk->getVector3Value();

        auto positionChunk = udaParser.getFirstChunk(true, POSITION_TAG, UdaAttribute(), shapeChunk);
        Point3<float> position = positionChunk->getPoint3Value();

        auto orientationChunk = udaParser.getFirstChunk(true, ORIENTATION_TAG, UdaAttribute(), shapeChunk);
        auto orientationAxisChunk = udaParser.getFirstChunk(true, AXIS_TAG, UdaAttribute(), orientationChunk);
        auto orientationAngleChunk = udaParser.getFirstChunk(true, ANGLE_TAG, UdaAttribute(), orientationChunk);
        Quaternion<float> orientation = Quaternion<float>::fromAxisAngle(orientationAxisChunk->getVector3Value(), orientationAngleChunk->getFloatValue());

        auto marginChunk = udaParser.getFirstChunk(true, MARGIN_TAG, UdaAttribute(), shapeChunk);
        float margin = marginChunk->getFloatValue();

        return std::make_unique<SoundBox>(halfSizes, position, orientation, margin);
    }

    void SoundBoxReaderWriter::write(UdaChunk& shapeChunk, const SoundShape& soundShape, UdaWriter& udaWriter) const {
        shapeChunk.addAttribute(UdaAttribute(TYPE_ATTR, BOX_VALUE));

        const auto& boxShape = static_cast<const SoundBox&>(soundShape);

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
