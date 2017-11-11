#ifndef URCHINENGINE_AIOBJECT_H
#define URCHINENGINE_AIOBJECT_H

#include <memory>
#include <vector>
#include <mutex>

#include "UrchinCommon.h"
#include "input/AIShape.h"

namespace urchin
{

	class AIObject
	{
		public:
			AIObject(std::string, const std::shared_ptr<AIShape> &, const Transform<float> &);
			AIObject(std::string, const std::vector<std::shared_ptr<AIShape>> &, const Transform<float> &);

			void setTransform(const Transform<float> &);

			const std::string &getName() const;
			const std::vector<std::shared_ptr<AIShape>> &getShapes() const;
			Transform<float> getTransform() const;

		private:
			mutable std::mutex mutex;

			std::string name;
			std::vector<std::shared_ptr<AIShape>> shapes;
			Transform<float> transform;
	};

}

#endif
