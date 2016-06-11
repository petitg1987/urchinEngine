#include "ActionReader.h"
#include "scene/controller/action/Point3Action.h"

namespace urchin
{

	ActionReader::ActionReader(const std::string &filename) :
			filename(filename)
	{

	}

	ActionReader::~ActionReader()
	{

	}

	std::vector<std::shared_ptr<Action>> ActionReader::readDebugFile() const
	{
		//TODO
	}

}
