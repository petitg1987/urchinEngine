#include <scene/renderer3d/camera/CameraSpaceService.h>

namespace urchin {

    CameraSpaceService::CameraSpaceService(const Camera& camera) :
            camera(camera) {

    }

    Point3<float> CameraSpaceService::worldSpacePointToViewSpace(const Point3<float>& worldSpacePoint) const {
        Point4<float> pointViewSpace = camera.getViewMatrix() * Point4(worldSpacePoint, 1.0f);
        return {pointViewSpace.X, pointViewSpace.Y, pointViewSpace.Z};
    }

    Point2<float> CameraSpaceService::worldSpacePointToNdcSpace(const Point3<float>& worldSpacePoint) const {
        Point4<float> pointClipSpace = camera.getProjectionViewMatrix() * Point4(worldSpacePoint, 1.0f);
        if (pointClipSpace.W != 0.0f) [[likely]] {
            return {pointClipSpace.X / pointClipSpace.W, pointClipSpace.Y / pointClipSpace.W};
        }
        return {pointClipSpace.X, pointClipSpace.Y};
    }

    Point2<float> CameraSpaceService::worldSpacePointToScreenSpace(const Point3<float>& worldSpacePoint) const {
        Point2<float> pointNdcSpace = worldSpacePointToNdcSpace(worldSpacePoint);
        return {((pointNdcSpace.X + 1.0f) / 2.0f) * ((float)camera.getSceneWidth()), ((pointNdcSpace.Y + 1.0f) / 2.0f) * ((float)camera.getSceneHeight())};
    }

    Point3<float> CameraSpaceService::screenPointToViewSpace(const Point2<float>& screenPoint) const {
        float ndcSpaceX = (2.0f * screenPoint.X) / ((float)camera.getSceneWidth()) - 1.0f;
        float ndcSpaceY = (2.0f * screenPoint.Y) / ((float)camera.getSceneHeight()) - 1.0f;
        Point4 pointNdcSpace(ndcSpaceX, ndcSpaceY, -1.0f /* NDC near plane */, 1.0f);
        Point4 pointViewSpace = camera.getProjectionInverseMatrix() * pointNdcSpace;
        return {pointViewSpace.X / pointViewSpace.W, pointViewSpace.Y / pointViewSpace.W, pointViewSpace.Z / pointViewSpace.W};
    }

    /**
     * Create a ray in world space from camera position to a screen point target.
     * @param screenPoint Point on screen defining the target of the ray. This point is generally defined by the cursor/sight position.
     */
    Ray<float> CameraSpaceService::screenPointToRay(const Point2<float>& screenPoint, float rayLength) const {
        Vector3<float> rayDirectionWorldSpace = screenPointToDirection(screenPoint);

        const Point3<float>& rayStart = camera.getPosition();
        Point3<float> rayEnd = rayStart.translate(rayDirectionWorldSpace * rayLength);
        return Ray(rayStart, rayEnd);
    }

    Line3D<float> CameraSpaceService::screenPointToLine(const Point2<float>& screenPoint) const {
        Vector3<float> lineDirectionWorldSpace = screenPointToDirection(screenPoint);

        const Point3<float>& point1 = camera.getPosition();
        Point3<float> point2 = point1.translate(lineDirectionWorldSpace);
        return Line3D(point1, point2);
    }

    Vector3<float> CameraSpaceService::screenPointToDirection(const Point2<float>& screenPoint) const {
        float ndcSpaceX = (2.0f * screenPoint.X) / ((float)camera.getSceneWidth()) - 1.0f;
        float ndcSpaceY = (2.0f * screenPoint.Y) / ((float)camera.getSceneHeight()) - 1.0f;
        Vector4 lineDirectionNdcSpace(ndcSpaceX, ndcSpaceY, -1.0f, 1.0f);
        Vector3<float> lineDirectionViewSpace = (camera.getProjectionInverseMatrix() * lineDirectionNdcSpace).xyz();
        return (camera.getViewMatrix().toMatrix3().inverse() * lineDirectionViewSpace).normalize();
    }
}
