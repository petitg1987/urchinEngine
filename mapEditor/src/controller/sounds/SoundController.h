#pragma once

#include <list>
#include <UrchinCommon.h>
#include <UrchinAggregation.h>
#include <UrchinSoundEngine.h>

#include <controller/AbstractController.h>

namespace urchin {

    class SoundController : public AbstractController {
        public:
            SoundController();

            std::list<const SoundEntity*> getSoundEntities() const;
            void addSoundEntity(std::unique_ptr<SoundEntity>);
            void removeSoundEntity(const SoundEntity&);

            void changeSoundShape(const SoundEntity&, SoundShape::ShapeType);
            void moveSoundInFrontOfCamera(const SoundEntity&, bool);

            const SoundEntity& updateSpatialSoundProperties(const SoundEntity&, const Point3<float>&, float);
            const SoundEntity& updateSoundTriggerGeneralProperties(const SoundEntity&, PlayBehavior);
            const SoundEntity& updateSoundShape(const SoundEntity&, std::unique_ptr<SoundShape>);

        private:
            SoundEntity& findSoundEntity(const SoundEntity&);
    };

}
