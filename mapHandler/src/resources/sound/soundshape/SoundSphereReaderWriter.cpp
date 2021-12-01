#include <resources/sound/soundshape/SoundSphereReaderWriter.h>

namespace urchin {

    std::unique_ptr<SoundShape> SoundSphereReaderWriter::loadFrom(const UdaChunk* shapeChunk, const UdaParser& udaParser) const {
        auto radiusChunk = udaParser.getUniqueChunk(true, RADIUS_TAG, UdaAttribute(), shapeChunk);
        float radius = radiusChunk->getFloatValue();

        auto positionChunk = udaParser.getUniqueChunk(true, POSITION_TAG, UdaAttribute(), shapeChunk);
        Point3<float> position = positionChunk->getPoint3Value();

        auto marginChunk = udaParser.getUniqueChunk(true, MARGIN_TAG, UdaAttribute(), shapeChunk);
        float margin = marginChunk->getFloatValue();

        return std::make_unique<SoundSphere>(radius, position, margin);
    }

    void SoundSphereReaderWriter::writeOn(UdaChunk& shapeChunk, const SoundShape& soundShape, UdaWriter& udaWriter) const {
        shapeChunk.addAttribute(UdaAttribute(TYPE_ATTR, SPHERE_VALUE));

        const auto& sphereShape = static_cast<const SoundSphere&>(soundShape);

        auto& radiusChunk = udaWriter.createChunk(RADIUS_TAG, UdaAttribute(), &shapeChunk);
        radiusChunk.setFloatValue(sphereShape.getRadius());

        auto& positionChunk = udaWriter.createChunk(POSITION_TAG, UdaAttribute(), &shapeChunk);
        positionChunk.setPoint3Value(sphereShape.getPosition());

        auto& marginChunk = udaWriter.createChunk(MARGIN_TAG, UdaAttribute(), &shapeChunk);
        marginChunk.setFloatValue(sphereShape.getMargin());
    }

}
