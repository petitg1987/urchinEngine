#include "AIWorld.h"

namespace urchin
{

    AIWorld::AIWorld()
    {
        objects.reserve(50); //estimated memory size
    }

	AIWorld::AIWorld(const std::shared_ptr<AIWorld> &aiWorld)
	{
		if(aiWorld)
		{
			objects = aiWorld->getObjects();
		}
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
