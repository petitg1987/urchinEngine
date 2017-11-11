#include <algorithm>

#include "AIWorld.h"

namespace urchin
{

    AIWorld::AIWorld()
    {
        objects.reserve(50); //estimated memory size
    }

	void AIWorld::addObject(const std::shared_ptr<AIObject> &aiObject)
	{
		std::lock_guard<std::mutex> lock(mutex);

		objects.push_back(aiObject);
	}

	void AIWorld::removeObject(const std::shared_ptr<AIObject> &aiObject)
	{
		std::lock_guard<std::mutex> lock(mutex);

		objects.erase(std::remove(objects.begin(), objects.end(), aiObject), objects.end());
	}

	std::vector<std::shared_ptr<AIObject>> AIWorld::getObjects() const
	{
		std::lock_guard<std::mutex> lock(mutex);

		return objects;
	}

}
