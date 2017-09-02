#ifndef URCHINENGINE_SPLITBOUNDINGBOX_H
#define URCHINENGINE_SPLITBOUNDINGBOX_H

#include <vector>
#include "UrchinCommon.h"

namespace urchin
{

	class SplitBoundingBox
	{
		public:
			SplitBoundingBox();
			~SplitBoundingBox();

			std::vector<AABBox<float>> split(const AABBox<float> &) const;

		private:
			const float limitSize;
	};

}

#endif
