#ifndef ENGINE_AIOBJECT_H
#define ENGINE_AIOBJECT_H

#include <memory>

#include "UrchinCommon.h"

namespace urchin
{

	class AIObject
	{
		public:
		AIObject(std::shared_ptr<ConvexShape3D>, const Transform<float> &);

		private:
			std::shared_ptr<ConvexShape3D> shape;
			Transform<float> transform;

			bool isLadder;

	};

}

#endif
