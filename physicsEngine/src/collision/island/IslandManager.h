#ifndef URCHINENGINE_ISLANDMANAGER_H
#define URCHINENGINE_ISLANDMANAGER_H

#include <vector>

#include "collision/island/IslandContainer.h"
#include "collision/ManifoldResult.h"
#include "body/BodyManager.h"
#include "body/work/WorkRigidBody.h"

namespace urchin
{

	class IslandManager
	{
		public:
			IslandManager(const BodyManager *);
			~IslandManager();

			void refreshBodyActiveState(const std::vector<ManifoldResult> &);

		private:
			void buildIslands(const std::vector<ManifoldResult> &);
			unsigned int computeNumberElements(const std::vector<IslandElementLink> &, unsigned int) const;
			bool isBodyMoving(const WorkRigidBody *) const;

			const BodyManager *bodyManager;
			IslandContainer islandContainer;

			const float squaredLinearSleepingThreshold;
			const float squaredAngularSleepingThreshold;

			#ifdef _DEBUG
				void printIslands(const std::vector<IslandElementLink> &);
			#endif
	};

}

#endif
