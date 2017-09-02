#ifndef URCHINENGINE_MAPEDITOR_SOUNDSHAPEWIDGETRETRIEVER_H
#define URCHINENGINE_MAPEDITOR_SOUNDSHAPEWIDGETRETRIEVER_H

#include <memory>
#include <QWidget>

#include "UrchinSoundEngine.h"
#include "UrchinMapHandler.h"
#include "scene/controller/sounds/soundshape/SoundShapeWidget.h"

namespace urchin
{

	class SoundShapeWidgetRetriever
	{
		public:
			SoundShapeWidgetRetriever(const SceneSound *);

			SoundShapeWidget *retrieveShapeWidget(const SoundShape *);
			SoundShapeWidget *retrieveShapeWidget(SoundShape::ShapeType);

		private:
			const SceneSound *sceneSound;
	};

}

#endif
