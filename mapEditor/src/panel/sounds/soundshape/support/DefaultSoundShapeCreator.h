#ifndef URCHINENGINE_MAPEDITOR_DEFAULTSOUNDSHAPECREATOR_H
#define URCHINENGINE_MAPEDITOR_DEFAULTSOUNDSHAPECREATOR_H

#include <memory>

#include "UrchinMapHandler.h"
#include "UrchinSoundEngine.h"

namespace urchin
{

	class DefaultSoundShapeCreator
	{

		public:
			explicit DefaultSoundShapeCreator(const SceneSound *);

			const SoundShape *createDefaultSoundShape(SoundShape::ShapeType) const;

		private:
			const SceneSound *sceneSound;
	};

}

#endif
