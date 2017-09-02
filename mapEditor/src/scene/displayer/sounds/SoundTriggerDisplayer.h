#ifndef URCHINENGINE_MAPEDITOR_SOUNDTRIGGERDISPLAYER_H
#define URCHINENGINE_MAPEDITOR_SOUNDTRIGGERDISPLAYER_H

#include "Urchin3dEngine.h"
#include "UrchinSoundEngine.h"
#include "UrchinMapHandler.h"

namespace urchin
{

	class SoundTriggerDisplayer
	{
		public:
			SoundTriggerDisplayer(SceneManager *);
			~SoundTriggerDisplayer();

			void displaySoundTriggerFor(const SceneSound *);

		private:
			GeometryModel *retrieveGeometry(const SoundShape *);

			GeometryModel *retrieveSphereGeometry(const SoundShape *) const;
			GeometryModel *retrieveBoxGeometry(const SoundShape *) const;

			void cleanCurrentDisplay();

			SceneManager *sceneManager;
			std::vector<GeometryModel *> soundTriggerModels;
	};

}

#endif
