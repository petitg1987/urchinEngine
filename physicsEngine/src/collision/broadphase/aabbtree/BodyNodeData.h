#ifndef URCHINENGINE_BODYNODEDATA_H
#define URCHINENGINE_BODYNODEDATA_H

#include <set>

#include "body/work/AbstractWorkBody.h"
#include "collision/broadphase/PairContainer.h"

namespace urchin
{

	class BodyNodeData
	{
		public:
			BodyNodeData(AbstractWorkBody *, PairContainer *);

			AbstractWorkBody *getBody() const;

			AABBox<float> retrieveBodyAABBox() const;

			bool hasAlternativePairContainer() const;
			PairContainer *getAlternativePairContainer() const;

			void addOwnerPairContainer(PairContainer *);
			std::set<PairContainer *> getOwnerPairContainers() const;

		private:
			AbstractWorkBody *body;

			PairContainer *alternativePairContainer;
			std::set<PairContainer *> ownerPairContainers;
	};

}

#endif
