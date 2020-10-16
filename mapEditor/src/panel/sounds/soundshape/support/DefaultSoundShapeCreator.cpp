#include "DefaultSoundShapeCreator.h"

namespace urchin {

    DefaultSoundShapeCreator::DefaultSoundShapeCreator(const SceneSound *sceneSound) :
        sceneSound(sceneSound) {

    }

    const SoundShape *DefaultSoundShapeCreator::createDefaultSoundShape(SoundShape::ShapeType shapeType) const {
        const Sound *sound = sceneSound->getSound();
        Point3<float> position(0.0, 0.0, 0.0);
        float radius = 1.0;
        if(const auto *pointSound = dynamic_cast<const PointSound *>(sound)) {
            position = pointSound->getPosition();
            radius = pointSound->getInaudibleDistance();
        }

        if(shapeType==SoundShape::ShapeType::SPHERE_SHAPE) {
            return new SoundSphere(radius, position, 0.0);
        } else if(shapeType==SoundShape::ShapeType::BOX_SHAPE) {
            Vector3<float> halfSizes(radius, radius, radius);
            Quaternion<float> orientation(Vector3<float>(0.0, 1.0, 0.0), 0.0);
            return new SoundBox(halfSizes, position, orientation, 0.0);
        }

        throw std::invalid_argument("Unknown shape type to create default sound shape: " + std::to_string(shapeType));
    }
}
