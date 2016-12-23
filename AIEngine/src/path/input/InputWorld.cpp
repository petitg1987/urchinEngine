#include "InputWorld.h"

namespace urchin
{

	InputWorld::InputWorld()
	{

	}

	void InputWorld::addObject(const InputObject &inputObject)
	{
		objects.push_back(inputObject);
	}

	const std::vector<InputObject> &InputWorld::getObjects() const
	{
		return objects;
	}

}
