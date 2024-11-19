#pragma once

#include <memory>
#include <UrchinSoundEngine.h>

namespace urchin {

    class DefaultSoundShapeCreator {

        public:
            explicit DefaultSoundShapeCreator(const Sound&);

            std::unique_ptr<SoundShape> createDefaultSoundShape(SoundShape::ShapeType) const;

        private:
            const Sound& sound;
    };

}
