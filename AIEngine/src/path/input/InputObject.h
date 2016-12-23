#ifndef ENGINE_INPUTOBJECT_H
#define ENGINE_INPUTOBJECT_H

#include <memory>

#include "UrchinCommon.h"

namespace urchin
{

	class InputObject
	{
		public:
			InputObject(std::shared_ptr<ConvexObject3D<float>>);

		private:
			std::shared_ptr<ConvexObject3D<float>> object;

			bool isLadder;

	};

}

#endif
