#include "CameraRayService.h"

namespace urchin
{

    CameraRayService::CameraRayService(Camera *camera) :
            camera(camera)
    {

    }

    /**
     * Create a ray in world space from camera position to a screen point target.
     * @param screenPoint Point on screen defining the target of the ray. This point is generally defined by the cursor/sight position.
     */
    Ray<float> CameraRayService::screenPointToRay(const Point2<float> &screenPoint, float rayLength) const
    {
        float clipSpaceX = (2.0f * (float)screenPoint.X) / ((float)camera->getSceneWidth()) - 1.0f;
        float clipSpaceY = 1.0f - (2.0f * (float)screenPoint.Y) / ((float)camera->getSceneHeight());
        urchin::Vector4<float> rayDirectionClipSpace(clipSpaceX, clipSpaceY, -1.0f, 1.0f);

        urchin::Vector4<float> rayDirectionEyeSpace = camera->getProjectionMatrix().inverse() * rayDirectionClipSpace;
        rayDirectionEyeSpace.setValues(rayDirectionEyeSpace.X, rayDirectionEyeSpace.Y, -1.0f, 0.0f);

        urchin::Vector3<float> rayDirectionWorldSpace = (camera->getViewMatrix().inverse() * rayDirectionEyeSpace).xyz().normalize();

        const urchin::Point3<float> &rayStart = camera->getPosition();
        urchin::Point3<float> rayEnd = rayStart.translate(rayDirectionWorldSpace * rayLength);
        return Ray<float>(rayStart, rayEnd);
    }
}
