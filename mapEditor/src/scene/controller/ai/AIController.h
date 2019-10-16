#ifndef URCHINENGINE_MAPEDITOR_AICONTROLLER_H
#define URCHINENGINE_MAPEDITOR_AICONTROLLER_H

#include "UrchinCommon.h"
#include "UrchinMapHandler.h"

namespace urchin
{

	class AIController
	{
		public:
			explicit AIController(MapHandler *);

			bool isModified() const;
			void resetModified();

			const SceneAI *getSceneAI() const;

			SceneAI *updateNavMeshAgent(const std::shared_ptr<NavMeshAgent>&);

		private:
			void markModified();

			bool bIsModified;
			MapHandler *mapHandler;
	};

}

#endif
