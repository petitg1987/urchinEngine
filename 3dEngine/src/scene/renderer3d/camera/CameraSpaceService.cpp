#include <scene/renderer3d/camera/CameraSpaceService.h>

namespace urchin {

    CameraSpaceService::CameraSpaceService(const Camera& camera) :
            camera(camera) {

    }

    Point2<float> CameraSpaceService::worldSpacePointToScreenSpace(const Point3<float>& worldSpacePoint) const {
        Point4<float> pointClipSpace = camera.getProjectionViewMatrix() * Point4<float>(worldSpacePoint, 1.0f);
        Point4<float> pointNdcSpace = pointClipSpace.divideByW();
        return Point2<float>(
                ((pointNdcSpace.X + 1.0f) / 2.0f) * ((float)camera.getSceneWidth()),
                ((pointNdcSpace.Y + 1.0f) / 2.0f) * ((float)camera.getSceneHeight()));
    }

    /**
     * Create a ray in world space from camera position to a screen point target.
     * @param screenPoint Point on screen defining the target of the ray. This point is generally defined by the cursor/sight position.
     */
    Ray<float> CameraSpaceService::screenPointToRay(const Point2<float>& screenPoint, float rayLength) const {
        Vector3<float> rayDirectionWorldSpace = screenPointToDirection(screenPoint);

        const Point3<float>& rayStart = camera.getPosition();
        Point3<float> rayEnd = rayStart.translate(rayDirectionWorldSpace * rayLength);
        return Ray<float>(rayStart, rayEnd);
    }

    Line3D<float> CameraSpaceService::screenPointToLine(const Point2<float>& screenPoint) const {
        Vector3<float> lineDirectionWorldSpace = screenPointToDirection(screenPoint);

        const Point3<float>& point1 = camera.getPosition();
        Point3<float> point2 = point1.translate(lineDirectionWorldSpace);
        return Line3D<float>(point1, point2);
    }

    Vector3<float> CameraSpaceService::screenPointToDirection(const Point2<float>& screenPoint) const {
        float clipSpaceX = (2.0f * screenPoint.X) / ((float)camera.getSceneWidth()) - 1.0f;
        float clipSpaceY = (2.0f * screenPoint.Y) / ((float)camera.getSceneHeight()) - 1.0f;
        Vector4 lineDirectionClipSpace(clipSpaceX, clipSpaceY, -1.0f, 1.0f);
        Vector3<float> lineDirectionEyeSpace = (camera.getProjectionInverseMatrix() * lineDirectionClipSpace).xyz();
        return (camera.getViewMatrix().toMatrix3().inverse() * lineDirectionEyeSpace).normalize();
    }
}
