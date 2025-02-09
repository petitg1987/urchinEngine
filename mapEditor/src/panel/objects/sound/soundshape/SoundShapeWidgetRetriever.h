#pragma once

#include <UrchinSoundEngine.h>
#include <panel/objects/sound/soundshape/SoundShapeWidget.h>

namespace urchin {

    class SoundShapeWidgetRetriever {
        public:
            SoundShapeWidget* retrieveShapeWidget(SoundShape::ShapeType) const;
    };

}
