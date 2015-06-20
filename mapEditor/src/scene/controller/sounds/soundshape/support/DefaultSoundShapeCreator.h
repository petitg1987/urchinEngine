#ifndef ENGINE_MAPEDITOR_DEFAULTSOUNDSHAPECREATOR_H
#define ENGINE_MAPEDITOR_DEFAULTSOUNDSHAPECREATOR_H

#include <memory>

#include "UrchinMapHandler.h"
#include "UrchinSoundEngine.h"

namespace urchin
{

	class DefaultSoundShapeCreator
	{

		public:
			DefaultSoundShapeCreator(const SceneSound *);

			const SoundShape *createDefaultSoundShape(SoundShape::ShapeType) const;

		private:
			const SceneSound *sceneSound;
	};

}

#endif
