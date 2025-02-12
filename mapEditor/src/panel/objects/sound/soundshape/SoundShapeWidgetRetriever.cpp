#include <stdexcept>

#include <panel/objects/sound/soundshape/SoundShapeWidgetRetriever.h>
#include <panel/objects/sound/soundshape/SoundShapeWidget.h>
#include <panel/objects/sound/soundshape/SoundSphereShapeWidget.h>
#include <panel/objects/sound/soundshape/SoundBoxShapeWidget.h>

namespace urchin {

    SoundShapeWidget* SoundShapeWidgetRetriever::retrieveShapeWidget(SoundShape::ShapeType shapeType, const ObjectEntity& objectEntity) const {
        if (shapeType == SoundShape::ShapeType::SPHERE_SHAPE) {
            return new SoundSphereShapeWidget(objectEntity);
        } else if (shapeType == SoundShape::ShapeType::BOX_SHAPE) {
            return new SoundBoxShapeWidget(objectEntity);
        }
        throw std::invalid_argument("Unknown shape type to retrieve sound shape widget: " + std::to_string(shapeType));
    }

}
