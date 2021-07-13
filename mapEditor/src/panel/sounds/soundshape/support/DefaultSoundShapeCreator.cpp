#include <panel/sounds/soundshape/support/DefaultSoundShapeCreator.h>

namespace urchin {

    DefaultSoundShapeCreator::DefaultSoundShapeCreator(const SceneSound& sceneSound) :
        sceneSound(sceneSound) {

    }

    std::unique_ptr<SoundShape> DefaultSoundShapeCreator::createDefaultSoundShape(SoundShape::ShapeType shapeType) const {
        const Sound* sound = sceneSound.getSound();
        Point3<float> position(0.0f, 0.0f, 0.0f);
        float radius = 1.0f;
        if (sound->getSoundType() == Sound::SoundType::SPATIAL){
            const auto* pointSound = dynamic_cast<const SpatialSound*>(sound);
            position = pointSound->getPosition();
            radius = pointSound->getInaudibleDistance();
        }

        if (shapeType == SoundShape::ShapeType::SPHERE_SHAPE) {
            return std::make_unique<SoundSphere>(radius, position, 0.0f);
        } else if (shapeType == SoundShape::ShapeType::BOX_SHAPE) {
            Vector3<float> halfSizes(radius, radius, radius);
            Quaternion<float> orientation(Vector3<float>(0.0f, 1.0f, 0.0f), 0.0f);
            return std::make_unique<SoundBox>(halfSizes, position, orientation, 0.0f);
        }

        throw std::invalid_argument("Unknown shape type to create default sound shape: " + std::to_string(shapeType));
    }
}
