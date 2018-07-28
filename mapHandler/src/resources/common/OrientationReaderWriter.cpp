#include <utility>

#include "OrientationReaderWriter.h"

namespace urchin
{

	Quaternion<float> OrientationReaderWriter::loadOrientation(std::shared_ptr<XmlChunk> parentChunk, const XmlParser &xmlParser) const
	{
		std::shared_ptr<XmlChunk> orientationChunk = xmlParser.getUniqueChunk(true, ORIENTATION_TAG, XmlAttribute(), std::move(parentChunk));

		std::shared_ptr<XmlChunk> orientationAxisChunk = xmlParser.getUniqueChunk(true, AXIS_TAG, XmlAttribute(), orientationChunk);
		std::shared_ptr<XmlChunk> orientationAngleChunk = xmlParser.getUniqueChunk(true, ANGLE_TAG, XmlAttribute(), orientationChunk);

		return Quaternion<float>(orientationAxisChunk->getVector3Value(), orientationAngleChunk->getFloatValue());
	}

	void OrientationReaderWriter::writeOrientation(std::shared_ptr<XmlChunk> parentChunk, const Quaternion<float> &orientation, XmlWriter &xmlWriter) const
	{
		std::shared_ptr<XmlChunk> orientationChunk = xmlWriter.createChunk(ORIENTATION_TAG, XmlAttribute(), std::move(parentChunk));

		std::shared_ptr<XmlChunk> orientationAxisChunk = xmlWriter.createChunk(AXIS_TAG, XmlAttribute(), orientationChunk);
		std::shared_ptr<XmlChunk> orientationAngleChunk = xmlWriter.createChunk(ANGLE_TAG, XmlAttribute(), orientationChunk);

		Vector3<float> orientationAxis;
		float orientationAngle;
		orientation.toAxisAngle(orientationAxis, orientationAngle);

		orientationAxisChunk->setVector3Value(orientationAxis);
		orientationAngleChunk->setFloatValue(orientationAngle);
	}

}
