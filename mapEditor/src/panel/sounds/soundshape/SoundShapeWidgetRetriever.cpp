#include <stdexcept>

#include <panel/sounds/soundshape/SoundShapeWidgetRetriever.h>
#include <panel/sounds/soundshape/SoundShapeWidget.h>
#include <panel/sounds/soundshape/SoundSphereShapeWidget.h>
#include <panel/sounds/soundshape/SoundBoxShapeWidget.h>

namespace urchin {

    SoundShapeWidgetRetriever::SoundShapeWidgetRetriever(const SceneSound& sceneSound) :
            sceneSound(sceneSound) {

    }

    std::unique_ptr<SoundShapeWidget> SoundShapeWidgetRetriever::retrieveShapeWidget(SoundShape::ShapeType shapeType) {
        std::unique_ptr<SoundShapeWidget> soundShapeWidget;

        if (shapeType == SoundShape::ShapeType::SPHERE_SHAPE) {
            soundShapeWidget = std::make_unique<SoundSphereShapeWidget>(sceneSound);
        } else if (shapeType == SoundShape::ShapeType::BOX_SHAPE) {
            soundShapeWidget = std::make_unique<SoundBoxShapeWidget>(sceneSound);
        } else {
            throw std::invalid_argument("Unknown shape type to retrieve sound shape widget: " + std::to_string(shapeType));
        }

        return soundShapeWidget;
    }

}
