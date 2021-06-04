#pragma once

#include <memory>

#include <UrchinMapHandler.h>
#include <UrchinSoundEngine.h>

namespace urchin {

    class DefaultSoundShapeCreator {

        public:
            explicit DefaultSoundShapeCreator(const SceneSound*);

            const SoundShape* createDefaultSoundShape(SoundShape::ShapeType) const;

        private:
            const SceneSound* sceneSound;
    };

}
