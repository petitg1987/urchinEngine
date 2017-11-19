#ifndef URCHINENGINE_AIOBJECT_H
#define URCHINENGINE_AIOBJECT_H

#include <memory>
#include <vector>
#include <mutex>
#include <atomic>

#include "UrchinCommon.h"
#include "input/AIShape.h"

namespace urchin
{

	class AIObject
	{
		public:
			AIObject(std::string, const std::shared_ptr<AIShape> &, const Transform<float> &, bool);
			AIObject(std::string, bool, const std::vector<std::shared_ptr<AIShape>> &, const Transform<float> &, bool);

			void updateTransform(const Point3<float> &, const Quaternion<float> &);
			bool isToRebuild() const;
			void markRebuilt();

			const std::string &getName() const;
			bool isDissociateShapes() const;
			const std::vector<std::shared_ptr<AIShape>> &getShapes() const;
			Transform<float> getTransform() const;
            bool isObstacleCandidate() const;

		private:
			mutable std::mutex mutex;
			std::atomic_bool bToRebuild;

			std::string name;
			bool bIsDissociateShapes;
			std::vector<std::shared_ptr<AIShape>> shapes;
			Transform<float> transform;
            bool bIsObstacleCandidate;
	};

}

#endif
