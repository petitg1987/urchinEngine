#include <stdexcept>

#include <panel/sounds/soundshape/SoundShapeWidgetRetriever.h>
#include <panel/sounds/soundshape/SoundShapeWidget.h>
#include <panel/sounds/soundshape/SoundSphereShapeWidget.h>
#include <panel/sounds/soundshape/SoundBoxShapeWidget.h>

namespace urchin {

    SoundShapeWidgetRetriever::SoundShapeWidgetRetriever(const SoundEntity& soundEntity) :
            soundEntity(soundEntity) {

    }

    SoundShapeWidget* SoundShapeWidgetRetriever::retrieveShapeWidget(SoundShape::ShapeType shapeType) const {
        if (shapeType == SoundShape::ShapeType::SPHERE_SHAPE) {
            return new SoundSphereShapeWidget(soundEntity);
        } else if (shapeType == SoundShape::ShapeType::BOX_SHAPE) {
            return new SoundBoxShapeWidget(soundEntity);
        }
        throw std::invalid_argument("Unknown shape type to retrieve sound shape widget: " + std::to_string(shapeType));
    }

}
