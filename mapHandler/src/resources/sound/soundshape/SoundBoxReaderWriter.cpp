#include "SoundBoxReaderWriter.h"

namespace urchin
{

	SoundBoxReaderWriter::SoundBoxReaderWriter()
	{

	}

	SoundBoxReaderWriter::~SoundBoxReaderWriter()
	{

	}

	SoundShape *SoundBoxReaderWriter::loadFrom(std::shared_ptr<XmlChunk> shapeChunk, const XmlParser &xmlParser) const
	{
		std::shared_ptr<XmlChunk> halfSizesChunk = xmlParser.getUniqueChunk(true, HALF_SIZES_TAG, XmlAttribute(), shapeChunk);
		Vector3<float> halfSizes = halfSizesChunk->getVector3Value();

		std::shared_ptr<XmlChunk> positionChunk = xmlParser.getUniqueChunk(true, POSITION_TAG, XmlAttribute(), shapeChunk);
		Point3<float> position = positionChunk->getPoint3Value();

		std::shared_ptr<XmlChunk> orientationChunk = xmlParser.getUniqueChunk(true, ORIENTATION_TAG, XmlAttribute(), shapeChunk);
		std::shared_ptr<XmlChunk> orientationAxisChunk = xmlParser.getUniqueChunk(true, AXIS_TAG, XmlAttribute(), orientationChunk);
		std::shared_ptr<XmlChunk> orientationAngleChunk = xmlParser.getUniqueChunk(true, ANGLE_TAG, XmlAttribute(), orientationChunk);
		Quaternion<float> orientation(orientationAxisChunk->getVector3Value(), orientationAngleChunk->getFloatValue());

		std::shared_ptr<XmlChunk> marginChunk = xmlParser.getUniqueChunk(true, MARGIN_TAG, XmlAttribute(), shapeChunk);
		float margin = marginChunk->getFloatValue();

		return new SoundBox(halfSizes, position, orientation, margin);
	}

	void SoundBoxReaderWriter::writeOn(std::shared_ptr<XmlChunk> shapeChunk, const SoundShape *soundShape, XmlWriter &xmlWriter) const
	{
		shapeChunk->setAttribute(XmlAttribute(TYPE_ATTR, BOX_VALUE));

		const SoundBox *boxshape = static_cast<const SoundBox *>(soundShape);

		std::shared_ptr<XmlChunk> halfSizesChunk = xmlWriter.createChunk(HALF_SIZES_TAG, XmlAttribute(), shapeChunk);
		halfSizesChunk->setVector3Value(boxshape->getHalfSizes());

		std::shared_ptr<XmlChunk> positionChunk = xmlWriter.createChunk(POSITION_TAG, XmlAttribute(), shapeChunk);
		positionChunk->setPoint3Value(boxshape->getCenterPosition());

		std::shared_ptr<XmlChunk> orientationChunk = xmlWriter.createChunk(ORIENTATION_TAG, XmlAttribute(), shapeChunk);
		std::shared_ptr<XmlChunk> orientationAxisChunk = xmlWriter.createChunk(AXIS_TAG, XmlAttribute(), orientationChunk);
		std::shared_ptr<XmlChunk> orientationAngleChunk = xmlWriter.createChunk(ANGLE_TAG, XmlAttribute(), orientationChunk);
		Vector3<float> orientationAxis;
		float orienationAngle;
		boxshape->getOrientation().toAxisAngle(orientationAxis, orienationAngle);
		orientationAxisChunk->setVector3Value(orientationAxis);
		orientationAngleChunk->setFloatValue(orienationAngle);

		std::shared_ptr<XmlChunk> marginChunk = xmlWriter.createChunk(MARGIN_TAG, XmlAttribute(), shapeChunk);
		marginChunk->setFloatValue(boxshape->getMargin());
	}

}
