#include <limits>

#include <math/geometry/3d/util/ShapeDetectService.h>
#include <math/geometry/3d/shape/BoxShape.h>
#include <math/geometry/3d/shape/SphereShape.h>
#include <math/geometry/3d/shape/ConvexHullShape3D.h>

namespace urchin {

	std::vector<ShapeDetectService::LocalizedShape> ShapeDetectService::detect(const std::vector<Point3<float>>& originalPoints) const {
		std::vector<LocalizedShape> result;
		Point3 position(0.0f, 0.0f, 0.0f);
		Quaternion<float> orientation;

		auto convexHullShape = std::make_unique<ConvexHullShape3D<float>>(originalPoints);
		std::vector<Point3<float>> convexPoints = convexHullShape->getPoints();

		std::unique_ptr<BoxShape<float>> boxShape = tryBuildBox(convexPoints, position, orientation);
		if (boxShape) {
			result.push_back({.shape = std::move(boxShape), .position = position, .orientation = orientation});
			return result;
		}

		std::unique_ptr<SphereShape<float>> sphereShape = tryBuildSphere(convexPoints, position, orientation);
		if (sphereShape) {
			result.push_back({.shape = std::move(sphereShape), .position = position, .orientation = orientation});
			return result;
		}

		result.push_back({.shape = std::move(convexHullShape), .position = position, .orientation = orientation});
		return result;
	}

	/**
	 * @param position [out] Shape position
	 * @param orientation [out] Shape orientation
	 */
	std::unique_ptr<BoxShape<float>> ShapeDetectService::tryBuildBox(const std::vector<Point3<float>>& convexPoints, Point3<float>& position, Quaternion<float>& orientation) const {
		constexpr float DIST_TO_CENTER_TOLERANCE_PERC = 0.025f;
		constexpr float ORTHOGONAL_TOLERANCE_DOT_PRODUCT = 0.05f;

		if (convexPoints.size() != 8) {
			return std::unique_ptr<BoxShape<float>>(nullptr);
		}

		Point3<float> cornerPoint = convexPoints[0];
		const auto [closestPointIndex, farthestPointIndex] = findClosestAndFarthestPoints(convexPoints, cornerPoint);

		Point3<float> boxCenterPoint = (cornerPoint + convexPoints[farthestPointIndex]) / 2.0f;
		float expectedDistanceToCenter = cornerPoint.distance(boxCenterPoint);
		float minExpectedDistanceToCenter = expectedDistanceToCenter - (expectedDistanceToCenter * DIST_TO_CENTER_TOLERANCE_PERC);
		float maxExpectedDistanceToCenter = expectedDistanceToCenter + (expectedDistanceToCenter * DIST_TO_CENTER_TOLERANCE_PERC);
		for (std::size_t i = 1; i < convexPoints.size(); ++i) {
			float distanceToCenter = convexPoints[i].distance(boxCenterPoint);
			if (distanceToCenter < minExpectedDistanceToCenter || distanceToCenter > maxExpectedDistanceToCenter) {
				return std::unique_ptr<BoxShape<float>>(nullptr);
			}
		}

		Vector3<float> xAxis = cornerPoint.vector(convexPoints[closestPointIndex]);
		std::array<std::size_t, 2> orthogonalVectorsToXAxis = {0, 0};
		for (std::size_t i = 1; i < convexPoints.size(); ++i) {
			if (i != closestPointIndex || i != farthestPointIndex) {
				Vector3<float> vector = cornerPoint.vector(convexPoints[i]).normalize();
				if (xAxis.normalize().dotProduct(vector) < ORTHOGONAL_TOLERANCE_DOT_PRODUCT) {
					if (orthogonalVectorsToXAxis[0] == 0) {
						orthogonalVectorsToXAxis[0] = i;
					} else if (orthogonalVectorsToXAxis[1] == 0) {
						orthogonalVectorsToXAxis[1] = i;
					} else if (cornerPoint.squareDistance(convexPoints[i]) < cornerPoint.squareDistance(convexPoints[orthogonalVectorsToXAxis[0]])) {
						orthogonalVectorsToXAxis[0] = i;
					} else if (cornerPoint.squareDistance(convexPoints[i]) < cornerPoint.squareDistance(convexPoints[orthogonalVectorsToXAxis[1]])) {
						orthogonalVectorsToXAxis[1] = i;
					}
				}
			}
		}
		if (orthogonalVectorsToXAxis[0] == 0 || orthogonalVectorsToXAxis[1] == 0) {
			return std::unique_ptr<BoxShape<float>>(nullptr);
		}

		Vector3<float> yAxis = cornerPoint.vector(convexPoints[orthogonalVectorsToXAxis[0]]);
		Vector3<float> zAxis = cornerPoint.vector(convexPoints[orthogonalVectorsToXAxis[1]]);
		Quaternion<float> xOrientation = Quaternion<float>::rotationFromTo(Vector3(1.0f, 0.0f, 0.0f), xAxis.normalize()).normalize();
		Quaternion<float> yOrientation = Quaternion<float>::rotationFromTo(xOrientation.rotateVector(Vector3(0.0f, 1.0f, 0.0f)), yAxis.normalize()).normalize();

		position = boxCenterPoint;
		orientation = yOrientation * xOrientation;
		return std::make_unique<BoxShape<float>>(Vector3(xAxis.length() / 2.0f, yAxis.length() / 2.0f, zAxis.length() / 2.0f));
	}

	/**
	 * @param position [out] Shape position
	 * @param orientation [out] Shape orientation
	 */
	std::unique_ptr<SphereShape<float>> ShapeDetectService::tryBuildSphere(const std::vector<Point3<float>>& convexPoints, Point3<float>& position, Quaternion<float>& orientation) const {
		constexpr float DIST_TO_CENTER_TOLERANCE_PERC = 0.025f;

		if (convexPoints.size() < 35 || convexPoints.size() > 2500) {
			std::unique_ptr<SphereShape<float>>(nullptr);
		}

		Point3<float> firstPoint = convexPoints[0];
		std::size_t farthestPointIndex = findFarthestPoint(convexPoints, firstPoint);

		Point3<float> sphereCenterPoint = (firstPoint + convexPoints[farthestPointIndex]) / 2.0f;
		float expectedRadius = firstPoint.distance(sphereCenterPoint);
		float minExpectedRadius = expectedRadius - (expectedRadius * DIST_TO_CENTER_TOLERANCE_PERC);
		float maxExpectedRadius = expectedRadius + (expectedRadius * DIST_TO_CENTER_TOLERANCE_PERC);
		for (std::size_t i = 1; i < convexPoints.size(); ++i) {
			float distanceToCenter = convexPoints[i].distance(sphereCenterPoint);
			if (distanceToCenter < minExpectedRadius || distanceToCenter > maxExpectedRadius) {
				return std::unique_ptr<SphereShape<float>>(nullptr);
			}
		}

		position = sphereCenterPoint;
		orientation = Quaternion<float>();
		return std::make_unique<SphereShape<float>>(expectedRadius);
	}

    std::pair<std::size_t, std::size_t> ShapeDetectService::findClosestAndFarthestPoints(const std::vector<Point3<float>>& points, const Point3<float>& refPoint) const {
		std::size_t closestPointIndex = 0;
	    std::size_t farthestPointIndex = 0;

	    float maxDistance = 0;
	    float minDistance = std::numeric_limits<float>::max();
	    for (std::size_t i = 0; i < points.size(); ++i) {
	        float distance = refPoint.squareDistance(points[i]);

	        if (distance < minDistance) {
	            minDistance = distance;
	            closestPointIndex = i;
	        }

	        if (distance > maxDistance) {
	            maxDistance = distance;
	            farthestPointIndex = i;
	        }
	    }

		return std::make_pair<>(closestPointIndex, farthestPointIndex);
	}

	std::size_t ShapeDetectService::findFarthestPoint(const std::vector<Point3<float>>& points, const Point3<float>& refPoint) const {
		std::size_t farthestPointIndex = 0;

		float maxDistance = 0;
		for (std::size_t i = 0; i < points.size(); ++i) {
			float distance = refPoint.squareDistance(points[i]);
			if (distance > maxDistance) {
				maxDistance = distance;
				farthestPointIndex = i;
			}
		}

		return farthestPointIndex;
	}

}
