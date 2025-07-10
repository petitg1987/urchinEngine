#include <limits>
#include <optional>

#include <math/geometry/3d/util/ShapeDetectService.h>
#include <math/geometry/3d/shape/BoxShape.h>
#include <math/geometry/3d/shape/SphereShape.h>
#include <math/geometry/3d/shape/ConvexHullShape3D.h>

#include "math/geometry/3d/voxel/VoxelService.h"

namespace urchin {

	std::vector<ShapeDetectService::LocalizedShape> ShapeDetectService::detect(const std::vector<Point3<float>>& vertices, const std::vector<unsigned int>& triangleIndices) const {
		std::vector<LocalizedShape> result;

		std::set uniqueVerticesSet(vertices.begin(), vertices.end());
		auto convexHullShape = std::make_unique<ConvexHullShape3D<float>>(std::vector(uniqueVerticesSet.begin(), uniqueVerticesSet.end()));
		std::vector<Point3<float>> convexPoints = convexHullShape->getPoints();

		std::optional<LocalizedShape> boxShape = tryBuildBox(convexPoints);
		if (boxShape.has_value()) {
			result.push_back(std::move(boxShape.value()));
			return result;
		}

		std::optional<LocalizedShape> sphereShape = tryBuildSphere(convexPoints);
		if (sphereShape.has_value()) {
			result.push_back(std::move(sphereShape.value()));
			return result;
		}

		std::vector<LocalizedShape> boxShapes = tryBuildAABBoxes(vertices, triangleIndices);
		if (!boxShapes.empty()) {
			for (LocalizedShape& box : boxShapes) {
				result.push_back(std::move(box));
			}
			return result;
		}

		result.push_back({
			.shape = std::move(convexHullShape),
			.position = Point3(0.0f, 0.0f, 0.0f),
			.orientation = Quaternion<float>()
		});
		return result;
	}

	std::optional<ShapeDetectService::LocalizedShape> ShapeDetectService::tryBuildBox(const std::vector<Point3<float>>& convexPoints) const {
		constexpr float DIST_TO_CENTER_TOLERANCE_PERC = 0.025f;
		constexpr float ORTHOGONAL_TOLERANCE_DOT_PRODUCT = 0.05f;

		if (convexPoints.size() != 8) {
			return std::nullopt;
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
				return std::nullopt;
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
			return std::nullopt;
		}

		Vector3<float> yAxis = cornerPoint.vector(convexPoints[orthogonalVectorsToXAxis[0]]);
		Vector3<float> zAxis = cornerPoint.vector(convexPoints[orthogonalVectorsToXAxis[1]]);
		Quaternion<float> xOrientation = Quaternion<float>::rotationFromTo(Vector3(1.0f, 0.0f, 0.0f), xAxis.normalize()).normalize();
		Quaternion<float> yOrientation = Quaternion<float>::rotationFromTo(xOrientation.rotateVector(Vector3(0.0f, 1.0f, 0.0f)), yAxis.normalize()).normalize();

		return std::make_optional<LocalizedShape>({
			.shape = std::make_unique<BoxShape<float>>(Vector3(xAxis.length() / 2.0f, yAxis.length() / 2.0f, zAxis.length() / 2.0f)),
			.position = boxCenterPoint,
			.orientation = yOrientation * xOrientation
		});
	}

	std::optional<ShapeDetectService::LocalizedShape> ShapeDetectService::tryBuildSphere(const std::vector<Point3<float>>& convexPoints) const {
		constexpr float DIST_TO_CENTER_TOLERANCE_PERC = 0.025f;

		if (convexPoints.size() < 35 || convexPoints.size() > 2500) {
			return std::nullopt;
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
				return std::nullopt;
			}
		}

		return std::make_optional<LocalizedShape>({
			.shape = std::make_unique<SphereShape<float>>(expectedRadius),
			.position = sphereCenterPoint,
			.orientation = Quaternion<float>()
		});
	}

	std::vector<ShapeDetectService::LocalizedShape> ShapeDetectService::tryBuildAABBoxes(const std::vector<Point3<float>>& vertices, const std::vector<unsigned int>& triangleIndices) const {
		std::vector<LocalizedShape> result;

		VoxelService voxelService;
		VoxelGrid voxelGrid = voxelService.voxelizeObject(0.1f, vertices, triangleIndices); //TODO avoid hard coded size
		std::vector<AABBox<float>> boxes = voxelService.voxelGridToAABBoxes(voxelGrid);

		for (const AABBox<float>& box : boxes) {
			result.push_back({
				.shape = std::make_unique<BoxShape<float>>(box.getHalfSizes()),
				.position = (box.getMin() + box.getMax()) / 2.0f,
				.orientation = Quaternion<float>()
			});
		}

		return result;
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
