#ifndef ENGINE_SWEEPANDPRUNE_H
#define ENGINE_SWEEPANDPRUNE_H

#include "UrchinCommon.h"

#include "body/work/AbstractWorkBody.h"
#include "collision/OverlappingPair.h"
#include "collision/broadphase/BroadPhaseAlgorithm.h"
#include "collision/broadphase/sweepandprune/BodyEndPoint.h"
#include "collision/broadphase/sweepandprune/BodyBox.h"
#include "collision/broadphase/PairContainer.h"

namespace urchin
{

	class SweepAndPrune : public BroadPhaseAlgorithm
	{
		public:
			SweepAndPrune();
			~SweepAndPrune();

			void addBody(AbstractWorkBody *, PairContainer *);
			void removeBody(AbstractWorkBody *);
			void updateBodies();

			const std::vector<OverlappingPair *> &getOverlappingPairs() const;

			std::vector<AbstractWorkBody *> rayTest(const Ray<float> &) const;
			std::vector<AbstractWorkBody *> bodyTest(const AbstractWorkBody *, const PhysicsTransform &, const PhysicsTransform &) const;

		private:
			std::vector<AbstractWorkBody *> enlargedRayTest(const Ray<float> &, float, const AbstractWorkBody *) const;

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
