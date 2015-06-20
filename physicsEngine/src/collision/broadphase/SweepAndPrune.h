#ifndef ENGINE_SWEEPANDPRUNE_H
#define ENGINE_SWEEPANDPRUNE_H

#include "UrchinCommon.h"

#include "collision/OverlappingPair.h"
#include "collision/broadphase/BodyEndPoint.h"
#include "collision/broadphase/BodyBox.h"
#include "collision/broadphase/PairContainer.h"

namespace urchin
{

	class SweepAndPrune
	{
		public:
			SweepAndPrune();
			~SweepAndPrune();

			void addBody(AbstractWorkBody *, PairContainer *);
			void removeBody(AbstractWorkBody *);
			void updateBodies();

			const std::vector<OverlappingPair *> &getOverlappingPairs() const;

		private:
			void createSentinels();
			float getMinSentinelValue();
			float getMaxSentinelValue();

			void sortMinDown(unsigned int, unsigned int, bool);
			void sortMaxDown(unsigned int, unsigned int, bool);
			void sortMinUp(unsigned int, unsigned int, bool);
			void sortMaxUp(unsigned int, unsigned int, bool);

			bool isOverlap(const BodyBox *, const BodyBox *, unsigned int, unsigned int);

			void createOverlappingPair(const BodyBox *, const BodyBox *);
			void removeOverlappingPair(const BodyBox *, const BodyBox *);

			std::map<AbstractWorkBody *, BodyBox *> bodiesBox;
			BodyEndPoint *sortedEndPoints[3];
			PairContainer *defaultPairContainer;

			const float contactBreakingThreshold;

			#ifdef _DEBUG
				void printSortedEndPoints();
			#endif
	};

}

#endif
