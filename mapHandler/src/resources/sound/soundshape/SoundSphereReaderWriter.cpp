#include "SoundSphereReaderWriter.h"

namespace urchin {

    std::unique_ptr<SoundShape> SoundSphereReaderWriter::loadFrom(const DataChunk* shapeChunk, const DataParser& dataParser) const {
        auto radiusChunk = dataParser.getUniqueChunk(true, RADIUS_TAG, DataAttribute(), shapeChunk);
        float radius = radiusChunk->getFloatValue();

        auto positionChunk = dataParser.getUniqueChunk(true, POSITION_TAG, DataAttribute(), shapeChunk);
        Point3<float> position = positionChunk->getPoint3Value();

        auto marginChunk = dataParser.getUniqueChunk(true, MARGIN_TAG, DataAttribute(), shapeChunk);
        float margin = marginChunk->getFloatValue();

        return std::make_unique<SoundSphere>(radius, position, margin);
    }

    void SoundSphereReaderWriter::writeOn(DataChunk& shapeChunk, const SoundShape& soundShape, DataWriter& dataWriter) const {
        shapeChunk.addAttribute(DataAttribute(TYPE_ATTR, SPHERE_VALUE));

        const auto& sphereShape = dynamic_cast<const SoundSphere&>(soundShape);

        auto& radiusChunk = dataWriter.createChunk(RADIUS_TAG, DataAttribute(), &shapeChunk);
        radiusChunk.setFloatValue(sphereShape.getRadius());

        auto& positionChunk = dataWriter.createChunk(POSITION_TAG, DataAttribute(), &shapeChunk);
        positionChunk.setPoint3Value(sphereShape.getPosition());

        auto& marginChunk = dataWriter.createChunk(MARGIN_TAG, DataAttribute(), &shapeChunk);
        marginChunk.setFloatValue(sphereShape.getMargin());
    }

}
