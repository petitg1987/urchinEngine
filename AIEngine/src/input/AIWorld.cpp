#include "AIWorld.h"

namespace urchin
{

	AIWorld::AIWorld()
	{

	}

	void AIWorld::addObject(const AIObject &aiObject)
	{
		objects.push_back(aiObject);
	}

	const std::vector<AIObject> &AIWorld::getObjects() const
	{
		return objects;
	}

}
