#include <panel/objects/sound/soundshape/support/DefaultSoundShapeCreator.h>

namespace urchin {

    DefaultSoundShapeCreator::DefaultSoundShapeCreator(const Sound& sound) :
            sound(sound) {

    }

    std::unique_ptr<SoundShape> DefaultSoundShapeCreator::createDefaultSoundShape(SoundShape::ShapeType shapeType) const {
        Point3 position(0.0f, 0.0f, 0.0f);
        float radius = 1.0f;
        if (sound.getSoundType() == Sound::SoundType::LOCALIZABLE) {
            const auto& localizableSound = static_cast<const LocalizableSound&>(sound);
            position = localizableSound.getPosition();
            radius = localizableSound.getRadius();
        }

        if (shapeType == SoundShape::ShapeType::SPHERE_SHAPE) {
            return std::make_unique<SoundSphere>(radius, position, 0.0f);
        } else if (shapeType == SoundShape::ShapeType::BOX_SHAPE) {
            Vector3 halfSizes(radius, radius, radius);
            return std::make_unique<SoundBox>(halfSizes, position, Quaternion<float>::rotationY(0.0f), 0.0f);
        }

        throw std::invalid_argument("Unknown shape type to create default sound shape: " + std::to_string(shapeType));
    }
}
