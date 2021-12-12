#include <utility>

#include <input/AIObject.h>
#include <input/AIEntity.h>

namespace urchin {

    AIObject::AIObject(std::string name, const Transform<float>& transform, bool bIsObstacleCandidate, std::unique_ptr<AIShape> shape) :
            AIEntity(std::move(name), transform, bIsObstacleCandidate) {
        shapes.push_back(std::move(shape));
    }

    AIObject::AIObject(std::string name, const Transform<float>& transform, bool bIsObstacleCandidate, std::vector<std::unique_ptr<AIShape>> shapes) :
            AIEntity(std::move(name), transform, bIsObstacleCandidate),
            shapes(std::move(shapes)) {

    }

    AIEntity::AIEntityType AIObject::getType() const {
        return AIEntity::OBJECT;
    }

    const std::vector<std::unique_ptr<AIShape>>& AIObject::getShapes() const {
        return shapes;
    }
}
