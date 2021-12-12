#pragma once

#include <memory>

#include <UrchinAggregation.h>
#include <UrchinSoundEngine.h>

namespace urchin {

    class DefaultSoundShapeCreator {

        public:
            explicit DefaultSoundShapeCreator(const SoundEntity&);

            std::unique_ptr<SoundShape> createDefaultSoundShape(SoundShape::ShapeType) const;

        private:
            const SoundEntity& soundEntity;
    };

}
