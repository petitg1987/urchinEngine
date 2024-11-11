#include <stdexcept>

#include <scene/sounds/SoundTriggerDisplayer.h>

namespace urchin {

    SoundTriggerDisplayer::SoundTriggerDisplayer(Scene& scene) :
            scene(scene) {

    }

    SoundTriggerDisplayer::~SoundTriggerDisplayer() {
        cleanCurrentDisplay();
    }

    void SoundTriggerDisplayer::displaySoundTrigger(const SoundEntity* soundEntity) {
        cleanCurrentDisplay();

        if (soundEntity) {
            const SoundTrigger& soundTrigger = soundEntity->getSoundComponent()->getSoundTrigger();

            if (soundTrigger.getTriggerType() == ZoneTrigger::TriggerType::ZONE_TRIGGER) {
                const auto& zoneTrigger = static_cast<const ZoneTrigger&>(soundTrigger);
                auto geometryModel = retrieveGeometry(zoneTrigger.getSoundShape());
                geometryModel->setPolygonMode(PolygonMode::WIREFRAME);
                soundTriggerModels.push_back(std::move(geometryModel));
            }

            for (const auto& soundTriggerModel : soundTriggerModels) {
                soundTriggerModel->setColor(0.0f, 1.0f, 1.0f);
                scene.getActiveRenderer3d()->getGeometryContainer().addGeometry(soundTriggerModel);
            }
        }
    }

    std::unique_ptr<GeometryModel> SoundTriggerDisplayer::retrieveGeometry(const SoundShape& soundShape) const {
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
