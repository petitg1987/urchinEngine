#ifndef ENGINE_DEBUGVISUALIZER_ACTION_H
#define ENGINE_DEBUGVISUALIZER_ACTION_H

#include <map>
#include <memory>
#include "Urchin3dEngine.h"

#include "scene/entity/GeometryEntityHandler.h"

namespace urchin
{

	class Action
	{
		public:
			Action();
			virtual ~Action();

			virtual void execute(std::shared_ptr<GeometryEntityHandler>) = 0;
			virtual void reverse(std::shared_ptr<GeometryEntityHandler>) = 0;
	};

}

#endif
