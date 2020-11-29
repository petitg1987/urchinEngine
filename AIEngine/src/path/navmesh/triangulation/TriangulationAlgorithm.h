#ifndef URCHINENGINE_TRIANGULATIONALGORITHM_H
#define URCHINENGINE_TRIANGULATIONALGORITHM_H

#include <vector>
#include <map>
#include "UrchinCommon.h"

#include "path/navmesh/model/output/NavTriangle.h"
#include "path/navmesh/triangulation/MonotonePolygon.h"

namespace urchin {

    struct SidedPoint {
        SidedPoint(std::size_t, bool);

        std::size_t pointIndex;
        bool onLeft; //indicate if point is on left of monotone polygon or right
    };

    struct TriangleEdge {
        TriangleEdge(std::size_t, std::size_t);

        std::size_t triangleIndex;
        std::size_t edgeIndex;
    };

    class TriangulationAlgorithm {
        public:
            TriangulationAlgorithm(std::vector<Point2<float>> &&, const std::string&);

            std::vector<Point2<float>> getPolygonPoints() const;

            std::size_t addHolePoints(const std::vector<Point2<float>>&, const std::string&);
            std::size_t getHolesSize() const;
            std::vector<Point2<float>> getHolePoints(std::size_t) const;

            std::size_t getAllPointsSize() const;

            const std::vector<std::shared_ptr<NavTriangle>> &triangulate();

        private:
            std::vector<std::shared_ptr<NavTriangle>> triangulateMonotonePolygon(const MonotonePolygon&);
            std::vector<SidedPoint> buildSortedSidedPoints(const std::vector<std::size_t>&) const;
            bool isFirstPointAboveSecond(std::size_t, std::size_t) const;
            std::shared_ptr<NavTriangle> buildCCWOrientedTriangle(std::size_t, std::size_t, std::size_t) const;

            void determineNeighbors(std::vector<std::shared_ptr<NavTriangle>> &, const MonotonePolygon&);
            void determineNeighborsInsideMonotone(std::vector<std::shared_ptr<NavTriangle>> &);
            void determineNeighborsBetweenMonotones(std::vector<std::shared_ptr<NavTriangle>> &, const MonotonePolygon&);
            bool areSameEdge(const std::shared_ptr<NavTriangle>&, std::size_t, std::size_t, const std::shared_ptr<NavTriangle>&, std::size_t, std::size_t) const;
            uint_fast64_t computeEdgeId(std::size_t, std::size_t) const;

            void logInputData(const std::string&, Logger::CriticalityLevel) const;
            void logOutputData(const std::string&, const std::vector<std::shared_ptr<NavTriangle>>&, Logger::CriticalityLevel) const;

            std::vector<Point2<float>> polygonPoints;
            std::vector<std::size_t> endContourIndices; //e.g.: 'polygonPoints' contains 5 CCW points and 4 CW points (hole). So, 'endContourIndices' will have values: 5 and 9.
            std::vector<std::string> contourNames;

            std::vector<std::shared_ptr<NavTriangle>> triangles;
            int missingTriangleNeighbor;
            std::map<uint_fast64_t, TriangleEdge> sharedMonotoneEdges;
    };

}

#endif
