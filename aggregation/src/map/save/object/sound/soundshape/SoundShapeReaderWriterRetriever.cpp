#include <stdexcept>

#include "map/save/object/sound/soundshape/SoundShapeReaderWriterRetriever.h"
#include "map/save/object/sound/soundshape/SoundSphereReaderWriter.h"
#include "map/save/object/sound/soundshape/SoundBoxReaderWriter.h"

namespace urchin {

    std::unique_ptr<SoundShapeReaderWriter> SoundShapeReaderWriterRetriever::retrieveShapeReaderWriter(const UdaChunk* shapeChunk) {
        std::string shapeType = shapeChunk->getAttributeValue(SoundShapeReaderWriter::TYPE_ATTR);
        if (shapeType == SoundShapeReaderWriter::SPHERE_VALUE) {
            return std::make_unique<SoundSphereReaderWriter>();
        } else if (shapeType == SoundShapeReaderWriter::BOX_VALUE) {
            return std::make_unique<SoundBoxReaderWriter>();
        }

        throw std::invalid_argument("Unknown shape type: " + shapeType);
    }

    std::unique_ptr<SoundShapeReaderWriter> SoundShapeReaderWriterRetriever::retrieveShapeReaderWriter(const SoundShape& soundShape) {
        SoundShape::ShapeType shapeType = soundShape.getShapeType();
        if (shapeType == SoundShape::SPHERE_SHAPE) {
            return std::make_unique<SoundSphereReaderWriter>();
        } else if (shapeType == SoundShape::BOX_SHAPE) {
            return std::make_unique<SoundBoxReaderWriter>();
        }

        throw std::invalid_argument("Unknown shape type: " + std::to_string(shapeType));
    }

}
