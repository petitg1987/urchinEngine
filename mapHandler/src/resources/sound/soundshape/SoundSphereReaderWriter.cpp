#include "SoundSphereReaderWriter.h"

namespace urchin {

    SoundShape* SoundSphereReaderWriter::loadFrom(const XmlChunk* shapeChunk, const DataParser& dataParser) const {
        auto radiusChunk = dataParser.getUniqueChunk(true, RADIUS_TAG, DataAttribute(), shapeChunk);
        float radius = radiusChunk->getFloatValue();

        auto positionChunk = dataParser.getUniqueChunk(true, POSITION_TAG, DataAttribute(), shapeChunk);
        Point3<float> position = positionChunk->getPoint3Value();

        auto marginChunk = dataParser.getUniqueChunk(true, MARGIN_TAG, DataAttribute(), shapeChunk);
        float margin = marginChunk->getFloatValue();

        return new SoundSphere(radius, position, margin);
    }

    void SoundSphereReaderWriter::writeOn(XmlChunk* shapeChunk, const SoundShape* soundShape, XmlWriter& xmlWriter) const {
        shapeChunk->setAttribute(DataAttribute(TYPE_ATTR, SPHERE_VALUE));

        const auto* sphereShape = dynamic_cast<const SoundSphere*>(soundShape);

        auto radiusChunk = xmlWriter.createChunk(RADIUS_TAG, DataAttribute(), shapeChunk);
        radiusChunk->setFloatValue(sphereShape->getRadius());

        auto positionChunk = xmlWriter.createChunk(POSITION_TAG, DataAttribute(), shapeChunk);
        positionChunk->setPoint3Value(sphereShape->getPosition());

        auto marginChunk = xmlWriter.createChunk(MARGIN_TAG, DataAttribute(), shapeChunk);
        marginChunk->setFloatValue(sphereShape->getMargin());
    }

}
