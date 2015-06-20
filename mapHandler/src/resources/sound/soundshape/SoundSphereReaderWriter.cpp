#include "SoundSphereReaderWriter.h"

namespace urchin
{

	SoundSphereReaderWriter::SoundSphereReaderWriter()
	{

	}

	SoundSphereReaderWriter::~SoundSphereReaderWriter()
	{

	}

	SoundShape *SoundSphereReaderWriter::loadFrom(std::shared_ptr<XmlChunk> shapeChunk, const XmlParser &xmlParser) const
	{
		std::shared_ptr<XmlChunk> radiusChunk = xmlParser.getUniqueChunk(true, RADIUS_TAG, XmlAttribute(), shapeChunk);
		float radius = radiusChunk->getFloatValue();

		std::shared_ptr<XmlChunk> positionChunk = xmlParser.getUniqueChunk(true, POSITION_TAG, XmlAttribute(), shapeChunk);
		Point3<float> position = positionChunk->getPoint3Value();

		std::shared_ptr<XmlChunk> marginChunk = xmlParser.getUniqueChunk(true, MARGIN_TAG, XmlAttribute(), shapeChunk);
		float margin = marginChunk->getFloatValue();

		return new SoundSphere(radius, position, margin);
	}

	void SoundSphereReaderWriter::writeOn(std::shared_ptr<XmlChunk> shapeChunk, const SoundShape *soundShape, XmlWriter &xmlWriter) const
	{
		shapeChunk->setAttribute(XmlAttribute(TYPE_ATTR, SPHERE_VALUE));

		const SoundSphere *sphereShape = static_cast<const SoundSphere *>(soundShape);

		std::shared_ptr<XmlChunk> radiusChunk = xmlWriter.createChunk(RADIUS_TAG, XmlAttribute(), shapeChunk);
		radiusChunk->setFloatValue(sphereShape->getRadius());

		std::shared_ptr<XmlChunk> positionChunk = xmlWriter.createChunk(POSITION_TAG, XmlAttribute(), shapeChunk);
		positionChunk->setPoint3Value(sphereShape->getPosition());

		std::shared_ptr<XmlChunk> marginChunk = xmlWriter.createChunk(MARGIN_TAG, XmlAttribute(), shapeChunk);
		marginChunk->setFloatValue(sphereShape->getMargin());
	}

}
