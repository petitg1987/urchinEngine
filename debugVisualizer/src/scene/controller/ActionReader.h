#ifndef ENGINE_DEBUGVISUALIZER_ACTIONREADER_H
#define ENGINE_DEBUGVISUALIZER_ACTIONREADER_H

#include <string>
#include <fstream>
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
			void nextLine(std::ifstream &file, std::string &buffer) const;

			std::shared_ptr<Action> readAction(std::ifstream &, const std::string &, const std::string &) const;
			std::shared_ptr<Action> readPoint3Action(std::ifstream &, const std::string &) const;
			std::shared_ptr<Action> readConvexHull3DAction(std::ifstream &, const std::string &) const;
			std::shared_ptr<Action> readClearEntityAction(const std::string &) const;

			std::string filename;

	};

}

#endif
