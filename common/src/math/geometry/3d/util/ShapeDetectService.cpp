#include <limits>

#include <math/geometry/3d/util/ShapeDetectService.h>
#include <math/geometry/3d/shape/BoxShape.h>
#include <math/geometry/3d/shape/SphereShape.h>
#include <math/geometry/3d/shape/ConvexHullShape3D.h>

namespace urchin {

    /**
	 * @param position [out] Shape position
	 * @param orientation [out] Shape orientation
	 */
	std::unique_ptr<ConvexShape3D<float>> ShapeDetectService::detect(const std::vector<Point3<float>>& shapeVertices, Point3<float>& position, Quaternion<float>& orientation) const {
		position = Point3(0.0f, 0.0f, 0.0f);
		orientation = Quaternion<float>();
		auto convexHullShape = std::make_unique<ConvexHullShape3D<float>>(shapeVertices);

		std::vector<Point3<float>> points = convexHullShape->getPoints();

		if (points.size() == 8) { //box shape ?
			constexpr float DIST_TO_CENTER_TOLERANCE_PERC = 0.025f;
			constexpr float ORTHOGONAL_TOLERANCE_DOT_PRODUCT = 0.05f;

			Point3<float> cornerPoint = points[0];
			const auto [closestPointIndex, farthestPointIndex] = findClosestAndFarthestPoints(points, cornerPoint);

			Point3<float> boxCenterPoint = (cornerPoint + points[farthestPointIndex]) / 2.0f;
			float expectedDistanceToCenter = cornerPoint.distance(boxCenterPoint);
			float minExpectedDistanceToCenter = expectedDistanceToCenter - (expectedDistanceToCenter * DIST_TO_CENTER_TOLERANCE_PERC);
			float maxExpectedDistanceToCenter = expectedDistanceToCenter + (expectedDistanceToCenter * DIST_TO_CENTER_TOLERANCE_PERC);
			for (std::size_t i = 1; i < points.size(); ++i) {
				float distanceToCenter = points[i].distance(boxCenterPoint);
				if (distanceToCenter < minExpectedDistanceToCenter || distanceToCenter > maxExpectedDistanceToCenter) {
					return convexHullShape;
				}
			}

			Vector3<float> xAxis = cornerPoint.vector(points[closestPointIndex]);
			std::array<std::size_t, 2> orthogonalVectorsToXAxis = {0, 0};
			for (std::size_t i = 1; i < points.size(); ++i) {
				if (i != closestPointIndex || i != farthestPointIndex) {
					Vector3<float> vector = cornerPoint.vector(points[i]).normalize();
					if (xAxis.normalize().dotProduct(vector) < ORTHOGONAL_TOLERANCE_DOT_PRODUCT) {
						if (orthogonalVectorsToXAxis[0] == 0) {
							orthogonalVectorsToXAxis[0] = i;
						} else if (orthogonalVectorsToXAxis[1] == 0) {
							orthogonalVectorsToXAxis[1] = i;
						} else if (cornerPoint.squareDistance(points[i]) < cornerPoint.squareDistance(points[orthogonalVectorsToXAxis[0]])) {
							orthogonalVectorsToXAxis[0] = i;
						} else if (cornerPoint.squareDistance(points[i]) < cornerPoint.squareDistance(points[orthogonalVectorsToXAxis[1]])) {
							orthogonalVectorsToXAxis[1] = i;
						}
					}
				}
			}
			if (orthogonalVectorsToXAxis[0] == 0 || orthogonalVectorsToXAxis[1] == 0) {
				return convexHullShape;
			}

			Vector3<float> yAxis = cornerPoint.vector(points[orthogonalVectorsToXAxis[0]]);
			Vector3<float> zAxis = cornerPoint.vector(points[orthogonalVectorsToXAxis[1]]);
			Quaternion<float> xOrientation = Quaternion<float>::rotationFromTo(Vector3(1.0f, 0.0f, 0.0f), xAxis.normalize()).normalize();
			Quaternion<float> yOrientation = Quaternion<float>::rotationFromTo(xOrientation.rotateVector(Vector3(0.0f, 1.0f, 0.0f)), yAxis.normalize()).normalize();

			position = boxCenterPoint;
			orientation = yOrientation * xOrientation;
			return std::make_unique<BoxShape<float>>(Vector3(xAxis.length() / 2.0f, yAxis.length() / 2.0f, zAxis.length() / 2.0f));
		}

		if (points.size() > 35 && points.size() < 2500) { //sphere shape ?
			constexpr float DIST_TO_CENTER_TOLERANCE_PERC = 0.025f;

			Point3<float> firstPoint = points[0];
			std::size_t farthestPointIndex = findFarthestPoint(points, firstPoint);

			Point3<float> sphereCenterPoint = (firstPoint + points[farthestPointIndex]) / 2.0f;
			float expectedRadius = firstPoint.distance(sphereCenterPoint);
			float minExpectedRadius = expectedRadius - (expectedRadius * DIST_TO_CENTER_TOLERANCE_PERC);
			float maxExpectedRadius = expectedRadius + (expectedRadius * DIST_TO_CENTER_TOLERANCE_PERC);
			for (std::size_t i = 1; i < points.size(); ++i) {
				float distanceToCenter = points[i].distance(sphereCenterPoint);
				if (distanceToCenter < minExpectedRadius || distanceToCenter > maxExpectedRadius) {
					return convexHullShape;
				}
			}

			position = sphereCenterPoint;
			orientation = Quaternion<float>();
			return std::make_unique<SphereShape<float>>(expectedRadius);
		}

		return convexHullShape;
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
