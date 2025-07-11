#include <limits>
#include <optional>
#include <unordered_map>
#include <queue>

#include <math/geometry/3d/util/ShapeDetectService.h>
#include <math/geometry/3d/shape/BoxShape.h>
#include <math/geometry/3d/shape/SphereShape.h>
#include <math/geometry/3d/voxel/VoxelService.h>

namespace urchin {

	ShapeDetectService::ShapeDetectService(Config config) :
			config(std::move(config)) {

	}

	std::vector<ShapeDetectService::LocalizedShape> ShapeDetectService::detect(const std::vector<Point3<float>>& vertices, const std::vector<unsigned int>& triangleIndices) const {
		std::vector<LocalizedShape> result;

		Mesh meshNoDuplicate = mergeDuplicatePoints(vertices, triangleIndices);
		std::vector<Mesh> meshes = splitDistinctMesh(meshNoDuplicate);

		for (const Mesh& mesh : meshes) {
			std::optional<LocalizedShape> boxShape = tryBuildBox(mesh.vertices);
			if (boxShape.has_value()) {
				result.push_back(std::move(boxShape.value()));
				continue;
			}

			std::optional<LocalizedShape> sphereShape = tryBuildSphere(mesh.vertices);
			if (sphereShape.has_value()) {
				result.push_back(std::move(sphereShape.value()));
				continue;
			}

			std::vector<LocalizedShape> boxShapes = tryBuildAABBoxes(mesh);
			if (!boxShapes.empty()) {
				for (LocalizedShape& box : boxShapes) {
					result.push_back(std::move(box));
				}
				//continue;
			}
		}

		return result;
	}

	ShapeDetectService::Mesh ShapeDetectService::mergeDuplicatePoints(const std::vector<Point3<float>>& vertices, const std::vector<unsigned int>& triangleIndices) const {
		std::unordered_map<Point3<float>, unsigned int, Point3<float>::Hash> pointToNewIndex;
		std::vector<unsigned int> oldToNewIndex(vertices.size());

		Mesh mesh = {};
		mesh.triangleIndices.resize(triangleIndices.size());

		for (std::size_t i = 0; i < vertices.size(); ++i) {
			const Point3<float>& point = vertices[i];
			auto itFind = pointToNewIndex.find(point);
			if (itFind == pointToNewIndex.end()) {
				unsigned int newIndex = (unsigned int)mesh.vertices.size();
				pointToNewIndex[point] = newIndex;
				mesh.vertices.push_back(point);
				oldToNewIndex[i] = newIndex;
			} else {
				oldToNewIndex[i] = itFind->second;
			}
		}

		for (std::size_t i = 0; i < triangleIndices.size(); ++i) {
			mesh.triangleIndices[i] = oldToNewIndex[triangleIndices[i]];
		}

		return mesh;
	}

	std::vector<ShapeDetectService::Mesh> ShapeDetectService::splitDistinctMesh(const Mesh& originalMesh) const { //TODO review
	    std::vector<Mesh> subMeshes;
	    if (originalMesh.triangleIndices.empty()) {
	        return subMeshes;
	    }

	    const unsigned int numTriangles = originalMesh.triangleIndices.size() / 3;
	    std::vector visitedTriangles(numTriangles, false);

	    // 1. Build adjacency (mapping original vertex index to list of triangle indices that use it)
	    // This allows finding triangles connected by a shared vertex
	    std::map<unsigned int, std::vector<unsigned int>> vertexToTriangles;
	    for (unsigned int i = 0; i < numTriangles; ++i) {
	        for (int j = 0; j < 3; ++j) {
	            unsigned int vertexIdx = originalMesh.triangleIndices[i * 3 + j];
	            vertexToTriangles[vertexIdx].push_back(i);
	        }
	    }

	    // 2. Iterate and perform BFS/DFS for connected components
	    for (unsigned int i = 0; i < numTriangles; ++i) {
	        if (!visitedTriangles[i]) {
	            // Start a new connected component (sub-mesh)
	            Mesh currentSubMesh;
	            std::map<unsigned int, unsigned int> originalToSubMeshVertexMap; // original_idx -> new_idx
	            unsigned int nextSubMeshVertexIdx = 0;

	            std::queue<unsigned int> q;
	            q.push(i);
	            visitedTriangles[i] = true;

	            while (!q.empty()) {
	                unsigned int currentTriangleIdx = q.front();
	                q.pop();

	                // Add vertices and triangle indices to the current sub-mesh
	                for (int j = 0; j < 3; ++j) {
	                    unsigned int originalVertexIdx = originalMesh.triangleIndices[currentTriangleIdx * 3 + j];

	                    if (originalToSubMeshVertexMap.contains(originalVertexIdx)) {
	                        // Vertex not yet in this sub-mesh, add it
	                        originalToSubMeshVertexMap[originalVertexIdx] = nextSubMeshVertexIdx++;
	                        currentSubMesh.vertices.push_back(originalMesh.vertices[originalVertexIdx]);
	                    }
	                    currentSubMesh.triangleIndices.push_back(originalToSubMeshVertexMap[originalVertexIdx]);
	                }

	                // Find connected triangles
	                std::set<unsigned int> neighbors; // Use set to avoid duplicate neighbors
	                for (int j = 0; j < 3; ++j) {
	                    unsigned int vertexIdx = originalMesh.triangleIndices[currentTriangleIdx * 3 + j];
	                    for (unsigned int neighborTriangleIdx : vertexToTriangles[vertexIdx]) {
	                        if (!visitedTriangles[neighborTriangleIdx]) {
	                            neighbors.insert(neighborTriangleIdx);
	                        }
	                    }
	                }

	                for (unsigned int neighborIdx : neighbors) {
	                    if (!visitedTriangles[neighborIdx]) {
	                        visitedTriangles[neighborIdx] = true;
	                        q.push(neighborIdx);
	                    }
	                }
	            }
	            subMeshes.push_back(currentSubMesh);
	        }
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
			return std::nullopt;
		}

		Vector3<float> yAxis = cornerPoint.vector(points[orthogonalVectorsToXAxis[0]]);
		Vector3<float> zAxis = cornerPoint.vector(points[orthogonalVectorsToXAxis[1]]);
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

	std::vector<ShapeDetectService::LocalizedShape> ShapeDetectService::tryBuildAABBoxes(const Mesh& mesh) const {
		std::vector<LocalizedShape> result;

		VoxelService voxelService;
		VoxelGrid voxelGrid = voxelService.voxelizeObject(config.voxelizationSize, mesh.vertices, mesh.triangleIndices);
		std::vector<AABBox<float>> boxes = voxelService.voxelGridToAABBoxes(voxelGrid);

		for (const AABBox<float>& box : boxes) {
			result.push_back({
				.shape = std::make_unique<BoxShape<float>>(box.getHalfSizes()),
				.position = box.getCenterOfMass(),
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
