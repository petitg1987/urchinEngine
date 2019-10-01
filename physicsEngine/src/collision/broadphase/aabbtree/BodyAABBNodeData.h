#ifndef URCHINENGINE_BODYAABBNODEDATA_H
#define URCHINENGINE_BODYAABBNODEDATA_H

#include <set>

#include "body/work/AbstractWorkBody.h"
#include "collision/broadphase/PairContainer.h"
#include "collision/broadphase/aabbtree/AABBNodeData.h"

namespace urchin
{

	class BodyAABBNodeData : public AABBNodeData<AbstractWorkBody>
	{
		public:
			BodyAABBNodeData(AbstractWorkBody *, PairContainer *);

            AABBNodeData<AbstractWorkBody> *clone() const override;

            const std::string &getObjectId() const override;
			AABBox<float> retrieveObjectAABBox() const override;
            bool isObjectMoving() const override;

			bool hasAlternativePairContainer() const;
			PairContainer *getAlternativePairContainer() const;

			void addOwnerPairContainer(PairContainer *);
			std::set<PairContainer *> getOwnerPairContainers() const;

		private:
			PairContainer *alternativePairContainer;
			std::set<PairContainer *> ownerPairContainers;
	};

}

#endif
