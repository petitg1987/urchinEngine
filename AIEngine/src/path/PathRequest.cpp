#include "PathRequest.h"

namespace urchin
{
    PathRequest::PathRequest(const Point3<float> &startPoint, const Point3<float> &endPoint) :
            startPoint(startPoint),
            endPoint(endPoint),
            bIsPathReady(false)
    {

    }

    const Point3<float> &PathRequest::getStartPoint() const
    {
        return startPoint;
    }

    const Point3<float> &PathRequest::getEndPoint() const
    {
        return endPoint;
    }

    void PathRequest::setPath(const std::vector<PathPoint> &path)
    {
        {
            std::lock_guard<std::mutex> lock(mutex);

            this->path = path;
        }

        bIsPathReady.store(true, std::memory_order_relaxed);
    }

    std::vector<PathPoint> PathRequest::getPath() const
    {
        std::lock_guard<std::mutex> lock(mutex);

        return path;
    }

    bool PathRequest::isPathReady() const
    {
        return bIsPathReady.load(std::memory_order_relaxed);
    }
}
