#ifndef ENGINE_MAPEDITOR_LIGHTSCOPEDISPLAYER_H
#define ENGINE_MAPEDITOR_LIGHTSCOPEDISPLAYER_H

#include "Urchin3dEngine.h"
#include "UrchinMapHandler.h"

namespace urchin
{

	class LightScopeDisplayer
	{
		public:
			LightScopeDisplayer(SceneManager *);
			~LightScopeDisplayer();

			void displayLightScopeFor(const SceneLight *);

		private:
			void cleanCurrentDisplay();

			SceneManager *sceneManager;
			std::vector<GeometryModel *> lightScopeModels;
	};

}

#endif
