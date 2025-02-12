#include <stdexcept>

#include <scene/object/sound/SoundScopeDisplayer.h>

namespace urchin {

    SoundScopeDisplayer::SoundScopeDisplayer(Scene& scene) :
            scene(scene) {

    }

    SoundScopeDisplayer::~SoundScopeDisplayer() {
        cleanCurrentDisplay();
    }

    void SoundScopeDisplayer::displaySoundScope(const ObjectEntity* objectEntity) {
        cleanCurrentDisplay();

        if (objectEntity) {
            const Sound& sound = objectEntity->getSoundComponent()->getSound();
            if (sound.getSoundType() == Sound::SoundType::LOCALIZABLE) {
                const auto& localizableSound = static_cast<const LocalizableSound&>(sound);
                auto geometryModel = std::make_unique<SphereModel>(Sphere(localizableSound.getRadius(), localizableSound.getPosition()), 13);
                geometryModel->setPolygonMode(PolygonMode::WIREFRAME);
                geometryModel->setColor(Vector3(0.0f, 0.0f, 1.0f));
                soundModels.push_back(std::move(geometryModel));
                scene.getActiveRenderer3d()->getGeometryContainer().addGeometry(soundModels.back());
            }

            const SoundTrigger& soundTrigger = objectEntity->getSoundComponent()->getSoundTrigger();
            if (soundTrigger.getTriggerType() == AreaTrigger::TriggerType::AREA_TRIGGER) {
                const auto& areaTrigger = static_cast<const AreaTrigger&>(soundTrigger);
                auto geometryModel = retrieveSoundShapeGeometry(areaTrigger.getSoundShape());
                geometryModel->setPolygonMode(PolygonMode::WIREFRAME);
                geometryModel->setColor(Vector3(0.0f, 1.0f, 1.0f));
                soundModels.push_back(std::move(geometryModel));
                scene.getActiveRenderer3d()->getGeometryContainer().addGeometry(soundModels.back());
            }
        }
    }

    std::unique_ptr<GeometryModel> SoundScopeDisplayer::retrieveSoundShapeGeometry(const SoundShape& soundShape) const {
        SoundShape::ShapeType shapeType = soundShape.getShapeType();
        if (shapeType == SoundShape::ShapeType::SPHERE_SHAPE) {
            const auto& soundSphere = static_cast<const SoundSphere&>(soundShape);
            return std::make_unique<SphereModel>(soundSphere.getPlayTriggerSphere(), 15);
        } else if (shapeType == SoundShape::ShapeType::BOX_SHAPE) {
            const auto& soundBox = static_cast<const SoundBox&>(soundShape);
            return std::make_unique<OBBoxModel>(soundBox.getPlayTriggerBox());
        }

        throw std::invalid_argument("Unknown shape type to retrieve geometry: " + std::to_string(shapeType));
    }

    void SoundScopeDisplayer::cleanCurrentDisplay() {
        for (const auto& soundModel : soundModels) {
            scene.getActiveRenderer3d()->getGeometryContainer().removeGeometry(*soundModel);
        }
        soundModels.clear();
    }
}
