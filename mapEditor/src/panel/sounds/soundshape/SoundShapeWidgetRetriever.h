#pragma once

#include <memory>
#include <QWidget>

#include <UrchinSoundEngine.h>
#include <UrchinMapHandler.h>
#include <panel/sounds/soundshape/SoundShapeWidget.h>

namespace urchin {

    class SoundShapeWidgetRetriever {
        public:
            explicit SoundShapeWidgetRetriever(const SoundEntity&);

            std::unique_ptr<SoundShapeWidget> retrieveShapeWidget(SoundShape::ShapeType);

        private:
            const SoundEntity& soundEntity;
    };

}
