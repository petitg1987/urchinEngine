#ifndef URCHINENGINE_SOUNDSHAPEWIDGETRETRIEVER_H
#define URCHINENGINE_SOUNDSHAPEWIDGETRETRIEVER_H

#include <memory>
#include <QWidget>

#include "UrchinSoundEngine.h"
#include "UrchinMapHandler.h"
#include "panel/sounds/soundshape/SoundShapeWidget.h"

namespace urchin {

    class SoundShapeWidgetRetriever {
        public:
            explicit SoundShapeWidgetRetriever(const SceneSound*);

            SoundShapeWidget *retrieveShapeWidget(const SoundShape*);
            SoundShapeWidget *retrieveShapeWidget(SoundShape::ShapeType);

        private:
            const SceneSound *sceneSound;
    };

}

#endif
