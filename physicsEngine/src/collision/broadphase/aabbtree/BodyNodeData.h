#ifndef ENGINE_BODYNODEDATA_H
#define ENGINE_BODYNODEDATA_H

#include "body/work/AbstractWorkBody.h"
#include "collision/broadphase/PairContainer.h"

namespace urchin
{

	class BodyNodeData
	{
		public:
			BodyNodeData(AbstractWorkBody *, PairContainer *);
			~BodyNodeData();

			AbstractWorkBody *getBody() const;

			AABBox<float> retrieveBodyAABBox() const;

		private:
			AbstractWorkBody *body;

			PairContainer *alternativePairContainer; //TODO use it...
	};

}

#endif
