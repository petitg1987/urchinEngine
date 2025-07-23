#pragma once

#include <UrchinSoundEngine.h>

#include "panel/object/sound/soundshape/SoundShapeWidget.h"

namespace urchin {

    class SoundShapeWidgetRetriever {
        public:
            SoundShapeWidget* retrieveShapeWidget(SoundShape::ShapeType, const ObjectEntity&) const;
    };

}
