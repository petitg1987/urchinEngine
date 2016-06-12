#ifndef ENGINE_DEBUGVISUALIZER_CLEARENTITYACTION_H
#define ENGINE_DEBUGVISUALIZER_CLEARENTITYACTION_H

#include <string>

#include "scene/controller/action/Action.h"
#include "scene/entity/GeometryEntityHandler.h"
#include "scene/entity/GeometryEntity.h"

namespace urchin
{

	class ClearEntityAction : public Action
	{
		public:
			ClearEntityAction(const std::string &);
			~ClearEntityAction();

			void execute(std::shared_ptr<GeometryEntityHandler>);
			void reverse(std::shared_ptr<GeometryEntityHandler>);

		private:
			std::string entityId;
			GeometryEntity *removedEntity;
	};

}

#endif
