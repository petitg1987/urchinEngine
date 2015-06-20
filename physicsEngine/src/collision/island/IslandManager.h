#ifndef ENGINE_ISLANDMANAGER_H
#define ENGINE_ISLANDMANAGER_H

#include <vector>

#include "collision/island/IslandContainer.h"
#include "body/BodyManager.h"
#include "body/work/WorkRigidBody.h"
#include "collision/OverlappingPair.h"

namespace urchin
{

	class IslandManager
	{
		public:
			IslandManager(const BodyManager *);
			~IslandManager();

			void refreshBodyActiveState(const std::vector<OverlappingPair *> &);

		private:
			void buildIslands(const std::vector<OverlappingPair *> &);
			unsigned int computeNumberElements(const std::vector<IslandElementLink> &, unsigned int) const;
			bool isBodyMoving(const WorkRigidBody *) const;

			const BodyManager *bodyManager;
			IslandContainer islandContainer;

			const float linearSleepingThreshold;
			const float angularSleepingThreshold;
	};

}

#endif
