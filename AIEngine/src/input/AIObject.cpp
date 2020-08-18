#include <utility>

#include "AIObject.h"
#include "AIEntity.h"

namespace urchin
{

    AIObject::AIObject(std::string name, const Transform<float> &transform, bool bIsObstacleCandidate, const std::shared_ptr<AIShape> &shape) :
            AIEntity(std::move(name), transform, bIsObstacleCandidate)
    {
        shapes.push_back(shape);
    }

    AIObject::AIObject(std::string name, const Transform<float> &transform, bool bIsObstacleCandidate, std::vector<std::shared_ptr<AIShape>> shapes) :
            AIEntity(std::move(name), transform, bIsObstacleCandidate),
            shapes(std::move(shapes))
    {

    }

    AIEntity::AIEntityType AIObject::getType() const
    {
        return AIEntity::OBJECT;
    }

    const std::vector<std::shared_ptr<AIShape>> &AIObject::getShapes() const
    {
        return shapes;
    }
}
