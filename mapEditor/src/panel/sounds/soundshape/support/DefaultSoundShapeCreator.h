#pragma once

#include <memory>

#include <UrchinMapHandler.h>
#include <UrchinSoundEngine.h>

namespace urchin {

    class DefaultSoundShapeCreator {

        public:
            explicit DefaultSoundShapeCreator(const SceneSound&);

            std::unique_ptr<SoundShape> createDefaultSoundShape(SoundShape::ShapeType) const;

        private:
            const SceneSound& sceneSound;
    };

}
