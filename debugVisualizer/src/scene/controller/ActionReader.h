#ifndef ENGINE_DEBUGVISUALIZER_ACTIONREADER_H
#define ENGINE_DEBUGVISUALIZER_ACTIONREADER_H

#include <string>
#include <memory>

#include "scene/controller/action/Action.h"

namespace urchin
{

	class ActionReader
	{
		public:
			ActionReader(const std::string &);
			~ActionReader();

			std::vector<std::shared_ptr<Action>> readDebugFile() const;

		private:
			std::string filename;

	};

}

#endif
