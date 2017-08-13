#ifndef ENGINE_MAPEDITOR_AICONTROLLER_H
#define ENGINE_MAPEDITOR_AICONTROLLER_H

#include "UrchinCommon.h"
#include "UrchinMapHandler.h"

namespace urchin
{

	class AIController
	{
		public:
			AIController(MapHandler *);
			~AIController() = default;

			bool isModified() const;
			void resetModified();

			const SceneAI *getSceneAI() const;

			SceneAI *updateNavMeshConfig(std::shared_ptr<NavMeshConfig>);

		private:
			void markModified();

			bool bIsModified;
			MapHandler *mapHandler;
	};

}

#endif
