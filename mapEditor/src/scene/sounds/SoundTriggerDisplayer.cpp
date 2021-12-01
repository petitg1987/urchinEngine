#include <stdexcept>

#include <scene/sounds/SoundTriggerDisplayer.h>

namespace urchin {

    SoundTriggerDisplayer::SoundTriggerDisplayer(Scene& scene) :
            scene(scene) {

    }

    SoundTriggerDisplayer::~SoundTriggerDisplayer() {
        cleanCurrentDisplay();
    }

    void SoundTriggerDisplayer::displaySoundTriggerFor(const SceneSound* sceneSound) {
        cleanCurrentDisplay();

        if (sceneSound) {
            const SoundTrigger* soundTrigger = sceneSound->getSoundTrigger();

            if (soundTrigger->getTriggerType() == ShapeTrigger::TriggerType::SHAPE_TRIGGER) {
                const auto& shapeTrigger = static_cast<const ShapeTrigger*>(soundTrigger);
                auto geometryModel = retrieveGeometry(shapeTrigger->getSoundShape());
                soundTriggerModels.push_back(std::move(geometryModel));
            }

            for (const auto& soundTriggerModel : soundTriggerModels) {
                soundTriggerModel->setColor(0.0f, 1.0f, 1.0f);
                scene.getActiveRenderer3d()->getGeometryContainer().addGeometry(soundTriggerModel);
            }
        }
    }

    std::unique_ptr<GeometryModel> SoundTriggerDisplayer::retrieveGeometry(const SoundShape& soundShape) {
        SoundShape::ShapeType shapeType = soundShape.getShapeType();
        if (shapeType == SoundShape::ShapeType::SPHERE_SHAPE) {
            return retrieveSphereGeometry(soundShape);
        } else if (shapeType == SoundShape::ShapeType::BOX_SHAPE) {
            return retrieveBoxGeometry(soundShape);
        }

        throw std::invalid_argument("Unknown shape type to retrieve geometry: " + std::to_string(shapeType));
    }

    std::unique_ptr<GeometryModel> SoundTriggerDisplayer::retrieveSphereGeometry(const SoundShape& soundShape) const {
        const auto& soundSphere = static_cast<const SoundSphere&>(soundShape);
        return std::make_unique<SphereModel>(soundSphere.getPlayTriggerSphere(), 15);
    }

    std::unique_ptr<GeometryModel> SoundTriggerDisplayer::retrieveBoxGeometry(const SoundShape& soundShape) const {
        const auto& soundBox = static_cast<const SoundBox&>(soundShape);
        return std::make_unique<OBBoxModel>(soundBox.getPlayTriggerBox());
    }

    void SoundTriggerDisplayer::cleanCurrentDisplay() {
        for (const auto& soundTriggerModel : soundTriggerModels) {
            scene.getActiveRenderer3d()->getGeometryContainer().removeGeometry(*soundTriggerModel);
        }
        soundTriggerModels.clear();
    }
}
