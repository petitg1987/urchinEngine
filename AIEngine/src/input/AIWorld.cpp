#include "AIWorld.h"

namespace urchin
{

	void AIWorld::addObject(const AIObject &aiObject)
	{
		objects.push_back(aiObject);
	}

	const std::vector<AIObject> &AIWorld::getObjects() const
	{
		return objects;
	}

}
