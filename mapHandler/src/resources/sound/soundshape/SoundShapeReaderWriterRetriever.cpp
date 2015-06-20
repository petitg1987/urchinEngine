#include <stdexcept>

#include "SoundShapeReaderWriterRetriever.h"
#include "resources/sound/soundshape/SoundSphereReaderWriter.h"
#include "resources/sound/soundshape/SoundBoxReaderWriter.h"

namespace urchin
{

	std::shared_ptr<SoundShapeReaderWriter> SoundShapeReaderWriterRetriever::retrieveShapeReaderWriter(std::shared_ptr<XmlChunk> shapeChunk)
	{
		std::string shapeType = shapeChunk->getAttributeValue(TYPE_ATTR);
		if(shapeType.compare(SPHERE_VALUE)==0)
		{
			return std::make_shared<SoundSphereReaderWriter>();
		}else if(shapeType.compare(BOX_VALUE)==0)
		{
			return std::make_shared<SoundBoxReaderWriter>();
		}

		throw std::invalid_argument("Unknown shape type: " + shapeType);
	}

	std::shared_ptr<SoundShapeReaderWriter> SoundShapeReaderWriterRetriever::retrieveShapeReaderWriter(const SoundShape *soundShape)
	{
		SoundShape::ShapeType shapeType = soundShape->getShapeType();
		if(shapeType==SoundShape::SPHERE_SHAPE)
		{
			return std::make_shared<SoundSphereReaderWriter>();
		}else if(shapeType==SoundShape::BOX_SHAPE)
		{
			return std::make_shared<SoundBoxReaderWriter>();
		}

		throw std::invalid_argument("Unknown shape type: " + shapeType);
	}

}
