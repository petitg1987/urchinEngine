#ifndef ENGINE_MAPEDITOR_SOUNDTRIGGERDISPLAYER_H
#define ENGINE_MAPEDITOR_SOUNDTRIGGERDISPLAYER_H

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
