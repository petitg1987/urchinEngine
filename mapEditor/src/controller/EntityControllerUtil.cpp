#include <controller/EntityControllerUtil.h>

namespace urchin {

    Point3<float> EntityControllerUtil::determineNewPosition(const Point3<float>& currentEntityPosition, bool isClonedEntity, const Camera& camera) {
        if (!isClonedEntity) { //new entity created from scratch
            return camera.getPosition().translate(camera.getView() * 5.0f);
        }

        bool entityInFrustum = camera.getFrustum().collideWithPoint(currentEntityPosition);
        bool entityNearToCamera = camera.getPosition().distance(currentEntityPosition) < ENTITY_NEAR_TO_CAMERA_DISTANCE;
        if (!entityInFrustum || !entityNearToCamera) {
            return camera.getPosition().translate(camera.getView() * 5.0f);
        }
        return Point3<float>(currentEntityPosition.X, currentEntityPosition.Y + 0.1f, currentEntityPosition.Z);
    }

}
