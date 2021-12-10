#include <stdexcept>

#include <panel/sounds/soundshape/SoundShapeWidgetRetriever.h>
#include <panel/sounds/soundshape/SoundShapeWidget.h>
#include <panel/sounds/soundshape/SoundSphereShapeWidget.h>
#include <panel/sounds/soundshape/SoundBoxShapeWidget.h>

namespace urchin {

    SoundShapeWidgetRetriever::SoundShapeWidgetRetriever(const SoundEntity& soundEntity) :
            soundEntity(soundEntity) {

    }

    std::unique_ptr<SoundShapeWidget> SoundShapeWidgetRetriever::retrieveShapeWidget(SoundShape::ShapeType shapeType) {
        if (shapeType == SoundShape::ShapeType::SPHERE_SHAPE) {
            return std::make_unique<SoundSphereShapeWidget>(soundEntity);
        } else if (shapeType == SoundShape::ShapeType::BOX_SHAPE) {
            return std::make_unique<SoundBoxShapeWidget>(soundEntity);
        }
        throw std::invalid_argument("Unknown shape type to retrieve sound shape widget: " + std::to_string(shapeType));
    }

}
