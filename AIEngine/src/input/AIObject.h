#ifndef ENGINE_AIOBJECT_H
#define ENGINE_AIOBJECT_H

#include <memory>

#include "UrchinCommon.h"

namespace urchin
{

	class AIObject
	{
		public:
			AIObject(const std::string &, std::shared_ptr<const ConvexShape3D<float>>, const Transform<float> &);

			const std::string &getName() const;

			std::shared_ptr<const ConvexShape3D<float>> getShape() const;
			Transform<float> getTransform() const;

		private:
			std::string name;

			std::shared_ptr<const ConvexShape3D<float>> shape;
			Transform<float> transform;
	};

}

#endif
