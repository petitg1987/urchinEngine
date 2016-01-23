#include "visualizer/CollisionVisualizer.h"

namespace urchin
{

#ifdef _DEBUG

	CollisionVisualizer::CollisionVisualizer(CollisionWorld *collisionWorld) :
		collisionWorld(collisionWorld)
	{
		collisionWorld->addObserver(this, CollisionWorld::COLLISION_RESULT_UPDATED);
	}

	CollisionVisualizer::~CollisionVisualizer()
	{
		collisionWorld->removeObserver(this, CollisionWorld::COLLISION_RESULT_UPDATED);
	}

	void CollisionVisualizer::notify(Observable *observable, int notificationType)
	{
		CollisionWorld *collisionWorld = dynamic_cast<CollisionWorld *>(observable);
		if(collisionWorld)
		{
			switch(notificationType)
			{
				case CollisionWorld::COLLISION_RESULT_UPDATED:
				{
					std::lock_guard<std::mutex> lock(visualizerDataMutex);

					const std::vector<ManifoldResult> *manifoldResults = collisionWorld->getLastUpdatedManifoldResults();
					this->manifoldResults.clear();
					for(unsigned int i=0; i<manifoldResults->size(); ++i)
					{
						this->manifoldResults.push_back(manifoldResults->at(i));
					}

					break;
				}
			}
		}
	}

	const std::vector<ManifoldResult> &CollisionVisualizer::getManifoldResult() const
	{
		std::lock_guard<std::mutex> lock(visualizerDataMutex);

		return manifoldResults;
	}

#endif

}
