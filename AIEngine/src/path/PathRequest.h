#ifndef URCHINENGINE_PATHREQUEST_H
#define URCHINENGINE_PATHREQUEST_H

#include <atomic>
#include <mutex>
#include "UrchinCommon.h"

namespace urchin
{

    class PathRequest
    {
        public:
            PathRequest(const Point3<float> &, const Point3<float> &);

            const Point3<float> &getStartPoint() const;
            const Point3<float> &getEndPoint() const;

            void setPath(const std::vector<Point3<float>> &);
            std::vector<Point3<float>> getPath() const;
            bool isPathReady() const;

        private:
            Point3<float> startPoint;
            Point3<float> endPoint;

            mutable std::mutex mutex;
            std::atomic_bool bIsPathReady;
            std::vector<Point3<float>> path;
    };

}

#endif
