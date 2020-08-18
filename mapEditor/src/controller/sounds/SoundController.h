#ifndef URCHINENGINE_SOUNDCONTROLLER_H
#define URCHINENGINE_SOUNDCONTROLLER_H

#include <list>
#include "UrchinCommon.h"
#include "UrchinMapHandler.h"
#include "UrchinSoundEngine.h"

#include "controller/AbstractController.h"

namespace urchin
{

    class SoundController : public AbstractController
    {
        public:
            SoundController();

            std::list<const SceneSound *> getSceneSounds() const;
            void addSceneSound(SceneSound *);
            void removeSceneSound(const SceneSound *);

            void changeSoundTrigger(const SceneSound *, SoundTrigger::TriggerType);
            void changeSoundShape(const SceneSound *, SoundShape::ShapeType);

            const SceneSound *updateSceneSoundGeneralProperties(const SceneSound *, float);
            const SceneSound *updateScenePointSoundProperties(const SceneSound *, const Point3<float> &, float);
            const SceneSound *updateSceneSoundBehaviorProperties(const SceneSound *, SoundBehavior::PlayBehavior, SoundBehavior::StopBehavior, float);
            const SceneSound *updateSceneSoundShape(const SceneSound *, const SoundShape *);

        private:
            SceneSound *findSceneSound(const SceneSound *);
    };

}

#endif
