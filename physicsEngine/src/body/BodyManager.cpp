#include <algorithm>

#include "body/BodyManager.h"
#include "body/RigidBody.h"

namespace urchin
{

	BodyManager::BodyManager() :
		lastUpdatedWorkBody(nullptr)
	{

	}

	BodyManager::~BodyManager()
	{
		for(std::vector<AbstractBody *>::iterator it = bodies.begin(); it!=bodies.end(); ++it)
		{
			delete *it;
		}

		for(std::vector<AbstractWorkBody *>::iterator it = workBodies.begin(); it!=workBodies.end(); ++it)
		{
			delete *it;
		}
	}

	void BodyManager::addBody(AbstractBody *body)
	{
		body->setIsNew(true);

		std::lock_guard<std::mutex> lock(bodiesMutex);
		this->bodies.push_back(body);
	}

	void BodyManager::removeBody(AbstractBody *body)
	{
		body->markAsDeleted();
	}

	AbstractWorkBody *BodyManager::getLastUpdatedWorkBody() const
	{
		return lastUpdatedWorkBody;
	}

	const std::vector<AbstractWorkBody *> &BodyManager::getWorkBodies() const
	{
		return workBodies;
	}

	/**
	 * Setup work bodies with new data on bodies
	 */
	void BodyManager::setupWorkBodies()
	{
		std::lock_guard<std::mutex> lock(bodiesMutex);

		std::vector<AbstractBody *>::iterator it = bodies.begin();
		while(it!=bodies.end())
		{
			bool bodyDeleted = false;
			AbstractBody *body = *it;
			AbstractWorkBody *workBody = (*it)->getWorkBody();

			if(body->isNew())
			{
				//create new work body
				workBody = body->createWorkBody();
				body->setWorkBody(workBody);
				body->setIsNew(false);
				workBodies.push_back(workBody);

				//update work body
				body->update(workBody);

				//add notification
				lastUpdatedWorkBody = workBody;
				notifyObservers(this, ADD_WORK_BODY);
			}else if(body->isDeleted())
			{
				std::vector<AbstractWorkBody *>::iterator itFind = std::find(workBodies.begin(), workBodies.end(), workBody);
				if(itFind!=workBodies.end())
				{
					//remove notification
					lastUpdatedWorkBody = workBody;
					notifyObservers(this, REMOVE_WORK_BODY);

					//delete work body
					delete workBody;
					workBodies.erase(itFind);
				}

				//delete body
				it = bodies.erase(it);
				delete body;
				bodyDeleted = true;
			}else
			{
				//update work body
				body->update(workBody);
			}

			if(!bodyDeleted)
			{
				++it;
			}
		}
	}

	void BodyManager::applyWorkBodies()
	{
		std::lock_guard<std::mutex> lock(bodiesMutex);

		for(std::vector<AbstractBody *>::iterator it = bodies.begin(); it!=bodies.end(); ++it)
		{
			AbstractBody *body = *it;
			AbstractWorkBody *workBody = (*it)->getWorkBody();
			if(workBody==nullptr)
			{ //work body is not created yet
				continue;
			}

			body->apply(workBody);
		}
	}

}
