#include <limits>
#include <optional>
#include <unordered_map>
#include <queue>
#include <map>
#include <ranges>
#include <set>
#include <cassert>

#include <math/geometry/3d/util/ShapeDetectService.h>
#include <math/geometry/3d/shape/BoxShape.h>
#include <math/geometry/3d/shape/SphereShape.h>
#include <math/geometry/3d/shape/ConvexHullShape3D.h>
#include <math/geometry/3d/voxel/VoxelService.h>

namespace urchin {

	ShapeDetectService::ShapeDetectService(Config config) :
			config(std::move(config)) {

	}

	std::vector<ShapeDetectService::LocalizedShape> ShapeDetectService::detect(const MeshData& mesh) const {
		#ifdef URCHIN_DEBUG
			std::unordered_set<Point3<float>, Point3<float>::Hash> pointsSet;
			for (const Point3<float>& vertex : mesh.getVertices()) {
				assert(pointsSet.insert(vertex).second);
			}
		#endif

		std::vector<MeshData> subMeshes = splitDistinctMeshes(mesh);

		std::vector<LocalizedShape> result;
		result.reserve(subMeshes.size());

		for (const MeshData& subMesh : subMeshes) {
			std::optional<LocalizedShape> boxShape = tryBuildBox(subMesh.getVertices());
			if (boxShape.has_value()) {
				result.push_back(std::move(boxShape.value()));
				continue;
			}

			std::optional<LocalizedShape> sphereShape = tryBuildSphere(subMesh.getVertices());
			if (sphereShape.has_value()) {
				result.push_back(std::move(sphereShape.value()));
				continue;
			}

			std::optional<LocalizedShape> convexHullShape = tryBuildConvexHull(subMesh);
			if (convexHullShape.has_value()) {
				result.push_back(std::move(convexHullShape.value()));
				continue;
			}

			std::vector<LocalizedShape> boxShapes = buildAABBoxes(subMesh);
			for (LocalizedShape& box : boxShapes) {
				result.push_back(std::move(box));
			}
		}

		return result;
	}

	std::vector<MeshData> ShapeDetectService::splitDistinctMeshes(const MeshData& mesh) const {
	    std::vector<MeshData> subMeshes;

		std::map<uint32_t, std::vector<std::size_t>> vertexToTriangles;
		for (std::size_t triangleIndex = 0; triangleIndex < mesh.getTrianglesIndices().size(); ++triangleIndex) {
		    for (int i = 0; i < 3; ++i) {
		        uint32_t vertexIndex = mesh.getTrianglesIndices()[triangleIndex][i];
		        vertexToTriangles[vertexIndex].push_back(triangleIndex);
		    }
		}

		std::vector visitedTriangles(mesh.getTrianglesIndices().size(), false);
		for (std::size_t triangleIndex = 0; triangleIndex < mesh.getTrianglesIndices().size(); ++triangleIndex) {
			if (visitedTriangles[triangleIndex]) {
				continue;
			}

			std::vector<Point3<float>> subMeshVertices;
			std::vector<std::array<uint32_t, 3>> subMeshTrianglesIndices;
			std::map<uint32_t, uint32_t> originalToSubMeshVertexMap;

			std::queue<std::size_t> trianglesQueue;
			trianglesQueue.push(triangleIndex);
			visitedTriangles[triangleIndex] = true;

			while (!trianglesQueue.empty()) {
				std::size_t currentTriangleIndex = trianglesQueue.front();
				trianglesQueue.pop();

				subMeshTrianglesIndices.push_back({0u, 0u, 0u});
				for (int i = 0; i < 3; ++i) {
					uint32_t originalVertexIndex = mesh.getTrianglesIndices()[currentTriangleIndex][i];
					if (!originalToSubMeshVertexMap.contains(originalVertexIndex)) {
						uint32_t subMeshVertexIndex = (uint32_t)subMeshVertices.size();
						originalToSubMeshVertexMap[originalVertexIndex] = subMeshVertexIndex;
						subMeshVertices.push_back(mesh.getVertices()[originalVertexIndex]);
					}
					subMeshTrianglesIndices.back()[i] = originalToSubMeshVertexMap[originalVertexIndex];
				}

				std::set<std::size_t> neighborTrianglesIndices;
				for (int i = 0; i < 3; ++i) {
				    uint32_t vertexIndex = mesh.getTrianglesIndices()[currentTriangleIndex][i];
				    for (std::size_t neighborTriangleIndex : vertexToTriangles[vertexIndex]) {
				        if (!visitedTriangles[neighborTriangleIndex]) {
				            neighborTrianglesIndices.insert(neighborTriangleIndex);
				        }
				    }
				}
				for (std::size_t neighborTriangleIndex : neighborTrianglesIndices) {
			        trianglesQueue.push(neighborTriangleIndex);
					visitedTriangles[neighborTriangleIndex] = true;
				}
			}

			subMeshes.emplace_back(subMeshVertices, subMeshTrianglesIndices);
		}

		return subMeshes;
	}

	std::optional<ShapeDetectService::LocalizedShape> ShapeDetectService::tryBuildBox(const std::vector<Point3<float>>& points) const {
		constexpr float DIST_TO_CENTER_TOLERANCE_PERC = 0.025f;
		constexpr float ORTHOGONAL_TOLERANCE_DOT_PRODUCT = 0.05f;

		if (points.size() != 8) {
			return std::nullopt;
		}

		Point3<float> cornerPoint = points[0];
		const auto [closestPointIndex, farthestPointIndex] = findClosestAndFarthestPoints(points, cornerPoint);

		Point3<float> boxCenterPoint = (cornerPoint + points[farthestPointIndex]) / 2.0f;
		float expectedDistanceToCenter = cornerPoint.distance(boxCenterPoint);
		float minExpectedDistanceToCenter = expectedDistanceToCenter - (expectedDistanceToCenter * DIST_TO_CENTER_TOLERANCE_PERC);
		float maxExpectedDistanceToCenter = expectedDistanceToCenter + (expectedDistanceToCenter * DIST_TO_CENTER_TOLERANCE_PERC);
		for (std::size_t i = 1; i < points.size(); ++i) {
			float distanceToCenter = points[i].distance(boxCenterPoint);
			if (distanceToCenter < minExpectedDistanceToCenter || distanceToCenter > maxExpectedDistanceToCenter) {
				return std::nullopt;
			}
		}

		//Compute X axis:
		Vector3<float> xAxis = cornerPoint.vector(points[closestPointIndex]);

		//Compute Y axis:
		std::size_t shortestOrthogonalVectorToXAxis = 0;
		for (std::size_t i = 1; i < points.size(); ++i) {
			Vector3<float> vector = cornerPoint.vector(points[i]).normalize();
			if (xAxis.normalize().dotProduct(vector) >= ORTHOGONAL_TOLERANCE_DOT_PRODUCT) {
				continue; //not orthogonal to X axis
			}
			if (shortestOrthogonalVectorToXAxis == 0 || cornerPoint.squareDistance(points[i]) < cornerPoint.squareDistance(points[shortestOrthogonalVectorToXAxis])) {
				shortestOrthogonalVectorToXAxis = i;
			}
		}
		if (shortestOrthogonalVectorToXAxis == 0) {
			return std::nullopt;
		}
		Vector3<float> yAxis = cornerPoint.vector(points[shortestOrthogonalVectorToXAxis]);

		//Compute Z axis:
		std::size_t shortestOrthogonalVectorToXYAxis = 0;
		for (std::size_t i = 1; i < points.size(); ++i) {
			if (i == shortestOrthogonalVectorToXAxis) {
				continue;
			}
			Vector3<float> vector = cornerPoint.vector(points[i]).normalize();
			if (xAxis.normalize().dotProduct(vector) >= ORTHOGONAL_TOLERANCE_DOT_PRODUCT || yAxis.normalize().dotProduct(vector) >= ORTHOGONAL_TOLERANCE_DOT_PRODUCT) {
				continue; //not orthogonal to X or Y axis
			}
			if (shortestOrthogonalVectorToXYAxis == 0 || cornerPoint.squareDistance(points[i]) < cornerPoint.squareDistance(points[shortestOrthogonalVectorToXYAxis])) {
				shortestOrthogonalVectorToXYAxis = i;
			}
		}
		if (shortestOrthogonalVectorToXYAxis == 0) {
			return std::nullopt;
		}
		Vector3<float> zAxis = cornerPoint.vector(points[shortestOrthogonalVectorToXYAxis]);

		Quaternion<float> xOrientation = Quaternion<float>::rotationFromTo(Vector3(1.0f, 0.0f, 0.0f), xAxis.normalize()).normalize();
		Quaternion<float> yOrientation = Quaternion<float>::rotationFromTo(xOrientation.rotateVector(Vector3(0.0f, 1.0f, 0.0f)), yAxis.normalize()).normalize();
		return std::make_optional<LocalizedShape>({
			.shape = std::make_unique<BoxShape<float>>(Vector3(xAxis.length() / 2.0f, yAxis.length() / 2.0f, zAxis.length() / 2.0f)),
			.position = boxCenterPoint,
			.orientation = yOrientation * xOrientation
		});
	}

	std::optional<ShapeDetectService::LocalizedShape> ShapeDetectService::tryBuildSphere(const std::vector<Point3<float>>& points) const {
		constexpr float DIST_TO_CENTER_TOLERANCE_PERC = 0.025f;

		if (points.size() < 35 || points.size() > 2500) {
			return std::nullopt;
		}

		Point3<float> firstPoint = points[0];
		std::size_t farthestPointIndex = findFarthestPoint(points, firstPoint);

		Point3<float> sphereCenterPoint = (firstPoint + points[farthestPointIndex]) / 2.0f;
		float expectedRadius = firstPoint.distance(sphereCenterPoint);
		float minExpectedRadius = expectedRadius - (expectedRadius * DIST_TO_CENTER_TOLERANCE_PERC);
		float maxExpectedRadius = expectedRadius + (expectedRadius * DIST_TO_CENTER_TOLERANCE_PERC);
		for (std::size_t i = 1; i < points.size(); ++i) {
			float distanceToCenter = points[i].distance(sphereCenterPoint);
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

	std::optional<ShapeDetectService::LocalizedShape> ShapeDetectService::tryBuildConvexHull(const MeshData& mesh) const {
		if (mesh.getVertices().size() > config.maxConvexHullPoints || !isConvexMesh(mesh)) {
			return std::nullopt;
		}

		try {
			return std::make_optional<LocalizedShape>({
				.shape = std::make_unique<ConvexHullShape3D<float>>(mesh.getVertices()),
				.position = Point3(0.0f, 0.0f, 0.0f),
				.orientation = Quaternion<float>()
			});
		} catch (const std::invalid_argument&) {
			//ignore build convex hull errors
		}

		return std::nullopt;
	}

	std::vector<ShapeDetectService::LocalizedShape> ShapeDetectService::buildAABBoxes(const MeshData& mesh) const {
		std::vector<LocalizedShape> result;
		std::vector<AABBox<float>> boxes;

		if (isManifoldMesh(mesh) && config.voxelizationEnabled) {
			VoxelService voxelService;
			VoxelGrid voxelGrid = voxelService.voxelizeManifoldMesh(config.voxelizationSize, mesh);
			boxes = voxelService.voxelGridToAABBoxes(voxelGrid);
		}

		if (boxes.empty()) {
			boxes = {computeAABBox(mesh.getVertices())};
		}

		for (const AABBox<float>& box : boxes) {
			result.push_back({
				.shape = std::make_unique<BoxShape<float>>(box.getHalfSizes()),
				.position = box.getCenterOfMass(),
				.orientation = Quaternion<float>()
			});
		}

		return result;
	}

	bool ShapeDetectService::isConvexMesh(const MeshData& mesh) const {
		constexpr float EPSILON = 0.01f;

		for (const std::array<uint32_t, 3>& triangleIndices : mesh.getTrianglesIndices()) {
			Point3<float> a = mesh.getVertices()[triangleIndices[0]];
			Point3<float> b = mesh.getVertices()[triangleIndices[1]];
			Point3<float> c = mesh.getVertices()[triangleIndices[2]];

			Vector3<float> normal = a.vector(c).crossProduct(a.vector(b)).normalize();

			for (std::size_t i = 0; i < mesh.getVertices().size(); ++i) {
				if (i == triangleIndices[0] || i == triangleIndices[1] || i == triangleIndices[2]) {
					continue;
				}

				Point3<float> point = mesh.getVertices()[i];
				float distance = normal.dotProduct(a.vector(point));

				if (distance > EPSILON) {
					return false;
				}
			}
		}

		return true;
	}

	bool ShapeDetectService::isManifoldMesh(const MeshData& mesh) const {
		const auto edgeIdProducer = [](uint32_t edgeIndex1, uint32_t edgeIndex2) {
			return edgeIndex1 < edgeIndex2 ? ((uint64_t)edgeIndex1 << 32 | (uint64_t)edgeIndex2) : ((uint64_t)edgeIndex2 << 32 | (uint64_t)edgeIndex1);
		};

		std::unordered_map<uint64_t, int> edgesCount;
		for (const std::array<uint32_t, 3>& triangleIndices : mesh.getTrianglesIndices()) {
			edgesCount[edgeIdProducer(triangleIndices[0], triangleIndices[1])]++;
			edgesCount[edgeIdProducer(triangleIndices[1], triangleIndices[2])]++;
			edgesCount[edgeIdProducer(triangleIndices[2], triangleIndices[0])]++;
		}

		for (uint64_t count : std::views::values(edgesCount)) {
			if (count != 2) {
				return false;
			}
		}
		return true;
	}

	AABBox<float> ShapeDetectService::computeAABBox(const std::vector<Point3<float>>& vertices) const {
		Point3 min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		Point3 max(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
		for (const Point3<float>& vertex : vertices) {
			min.X = std::min(min.X, vertex.X);
			min.Y = std::min(min.Y, vertex.Y);
			min.Z = std::min(min.Z, vertex.Z);

			max.X = std::max(max.X, vertex.X);
			max.Y = std::max(max.Y, vertex.Y);
			max.Z = std::max(max.Z, vertex.Z);
		}
		return AABBox(min, max);
	}

    std::pair<std::size_t, std::size_t> ShapeDetectService::findClosestAndFarthestPoints(const std::vector<Point3<float>>& points, const Point3<float>& refPoint) const {
		std::size_t closestPointIndex = 0;
	    std::size_t farthestPointIndex = 0;

	    float maxDistance = 0;
	    float minDistance = std::numeric_limits<float>::max();
	    for (std::size_t i = 0; i < points.size(); ++i) {
			if (points[i].isEqual(refPoint, std::numeric_limits<float>::min())) {
				continue;
			}

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
