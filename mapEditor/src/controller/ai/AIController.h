#ifndef URCHINENGINE_AICONTROLLER_H
#define URCHINENGINE_AICONTROLLER_H

#include "UrchinCommon.h"
#include "UrchinMapHandler.h"

#include "controller/AbstractController.h"

namespace urchin
{

	class AIController : public AbstractController
	{
		public:
			explicit AIController(MapHandler *);

			const SceneAI *getSceneAI() const;

			const SceneAI *updateSceneAI(const std::shared_ptr<NavMeshAgent> &);
	};

}

#endif
