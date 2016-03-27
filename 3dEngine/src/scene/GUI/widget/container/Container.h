#ifndef ENGINE_CONTAINER_H
#define ENGINE_CONTAINER_H

#include "scene/GUI/widget/Widget.h"
#include "scene/GUI/widget/Position.h"

namespace urchin
{

	/**
	 * Container of widgets
	 */
	class Container : public Widget
	{
		public:
			Container(Position);
			virtual ~Container();

			void createOrUpdateWidget();
	};

}

#endif
