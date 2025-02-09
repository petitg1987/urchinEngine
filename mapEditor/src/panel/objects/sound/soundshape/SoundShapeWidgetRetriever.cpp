#include <stdexcept>

#include <panel/objects/sound/soundshape/SoundShapeWidgetRetriever.h>
#include <panel/objects/sound/soundshape/SoundShapeWidget.h>
#include <panel/objects/sound/soundshape/SoundSphereShapeWidget.h>
#include <panel/objects/sound/soundshape/SoundBoxShapeWidget.h>

namespace urchin {

    SoundShapeWidget* SoundShapeWidgetRetriever::retrieveShapeWidget(SoundShape::ShapeType shapeType) const {
        if (shapeType == SoundShape::ShapeType::SPHERE_SHAPE) {
            return new SoundSphereShapeWidget();
        } else if (shapeType == SoundShape::ShapeType::BOX_SHAPE) {
            return new SoundBoxShapeWidget();
        }
        throw std::invalid_argument("Unknown shape type to retrieve sound shape widget: " + std::to_string(shapeType));
    }

}
