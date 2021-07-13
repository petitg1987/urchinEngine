#include <stdexcept>

#include <panel/sounds/soundshape/SoundShapeWidgetRetriever.h>
#include <panel/sounds/soundshape/SoundShapeWidget.h>
#include <panel/sounds/soundshape/SoundSphereShapeWidget.h>
#include <panel/sounds/soundshape/SoundBoxShapeWidget.h>

namespace urchin {

    SoundShapeWidgetRetriever::SoundShapeWidgetRetriever(const SceneSound& sceneSound) :
            sceneSound(sceneSound) {

    }

    SoundShapeWidget* SoundShapeWidgetRetriever::retrieveShapeWidget(SoundShape::ShapeType shapeType) {
        SoundShapeWidget *soundShapeWidget;

        if (shapeType == SoundShape::ShapeType::SPHERE_SHAPE) {
            soundShapeWidget = new SoundSphereShapeWidget(sceneSound);
        } else if (shapeType == SoundShape::ShapeType::BOX_SHAPE) {
            soundShapeWidget = new SoundBoxShapeWidget(sceneSound);
        } else {
            throw std::invalid_argument("Unknown shape type to retrieve sound shape widget: " + std::to_string(shapeType));
        }

        return soundShapeWidget;
    }

}
