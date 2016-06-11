#ifndef ENGINE_DEBUGVISUALIZER_SCENECONTROLLER_H
#define ENGINE_DEBUGVISUALIZER_SCENECONTROLLER_H

#include <string>
#include <memory>
#include <vector>

#include "scene/entity/GeometryEntityHandler.h"
#include "scene/controller/action/Action.h"

namespace urchin
{

	class SceneController
	{
		public:
			SceneController();
			~SceneController();

			void openDebugFile(const std::string &, std::shared_ptr<GeometryEntityHandler>);

			bool hasDebugFileOpen();

			void executeNextAction();
			void reverseLastAction();

		private:
			bool bHasDebugFileOpen;

			std::shared_ptr<GeometryEntityHandler> geometryEntityHandler;

			std::vector<std::shared_ptr<Action>> actions;
			std::vector<std::shared_ptr<Action>>::const_iterator actionsIt;
	};

}

#endif
