#pragma once

#include <memory>

#include <UrchinSoundEngine.h>
#include <UrchinAggregation.h>
#include <panel/sounds/soundshape/SoundShapeWidget.h>

namespace urchin {

    class SoundShapeWidgetRetriever {
        public:
            explicit SoundShapeWidgetRetriever(const SoundEntity&);

            SoundShapeWidget* retrieveShapeWidget(SoundShape::ShapeType) const;

        private:
            const SoundEntity& soundEntity;
    };

}
