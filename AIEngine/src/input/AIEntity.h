#ifndef URCHINENGINE_AIENTITY_H
#define URCHINENGINE_AIENTITY_H

#include "UrchinCommon.h"

#include <string>
#include <mutex>
#include <atomic>


namespace urchin
{

    class AIEntity
    {
        public:
            enum AIEntityType
            {
                OBJECT,
                TERRAIN
            };

            AIEntity(std::string, const Transform<float> &, bool);
            virtual ~AIEntity() = default;

            void updateTransform(const Point3<float> &, const Quaternion<float> &);
            bool isToRebuild() const;
            void markRebuilt();

            virtual AIEntityType getType() const = 0;

            const std::string &getName() const;
            Transform<float> getTransform() const;
            bool isObstacleCandidate() const;

        private:
            mutable std::mutex mutex;
            std::atomic_bool bToRebuild;

            std::string name;
            Transform<float> transform;
            bool bIsObstacleCandidate;
    };

}

#endif
