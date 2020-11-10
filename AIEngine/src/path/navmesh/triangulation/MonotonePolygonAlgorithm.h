#ifndef URCHINENGINE_MONOTONEPOLYGONALGORITHM_H
#define URCHINENGINE_MONOTONEPOLYGONALGORITHM_H

#include <vector>
#include <stdexcept>
#include <map>
#include "UrchinCommon.h"

#include "path/navmesh/triangulation/MonotonePolygon.h"

namespace urchin {

    enum PointType {
        START_VERTEX, //neighbor points go down and interior angle <pi
        END_VERTEX, //neighbor points go up and interior angle <pi
        SPLIT_VERTEX, //neighbor points go down and interior angle >=pi
        MERGE_VERTEX, //neighbor points go up and interior angle >=pi
        REGULAR_DOWN_VERTEX, //previous neighbor is upper and next neighbor is lower
        REGULAR_UP_VERTEX //previous neighbor is lower and next neighbor is upper
    };

    struct TypedPoint {
        TypedPoint(std::size_t, PointType);

        std::size_t pointIndex;
        PointType type;
    };

    struct Edge {
        Edge(std::size_t, std::size_t);

        std::size_t startIndex;
        std::size_t endIndex;

        bool isProcessed;
    };

    struct EdgeHelper {
        EdgeHelper(Edge, std::size_t, PointType);

        Edge edge;

        std::size_t helperPointIndex;
        PointType helperPointType;
    };

    class MonotonePolygonError : public std::runtime_error {
        public:
            explicit MonotonePolygonError(const std::string &message);
    };

    class MonotonePolygonAlgorithm {
        public:
            MonotonePolygonAlgorithm(const std::vector<Point2<float>> &, const std::vector<std::size_t> &, const std::vector<std::string> &);

            std::vector<MonotonePolygon> createYMonotonePolygons();

        private:
            typedef std::multimap<std::size_t, Edge>::iterator it_diagonals;

            void createYMonotonePolygonsDiagonals();
            std::vector<TypedPoint> buildSortedTypedPoints(bool &) const;
            bool isFirstPointAboveSecond(std::size_t, std::size_t) const;

            void handleStartVertex(std::size_t);
            void handleSplitVertex(std::size_t);
            void handleEndVertex(std::size_t);
            void handleMergeVertex(std::size_t);
            void handleRegularDownVertex(std::size_t);
            void handleRegularUpVertex(std::size_t);

            std::size_t nextPointIndex(std::size_t) const;
            std::size_t previousPointIndex(std::size_t) const;

            void createEdgeHelper(std::size_t, std::size_t, PointType);
            std::vector<EdgeHelper>::iterator findEdgeHelper(std::size_t);
            std::vector<EdgeHelper>::iterator findNearestLeftEdgeHelper(std::size_t);
            void createDiagonals(std::size_t, std::size_t);

            std::size_t retrieveNextPointIndex(std::size_t, std::size_t, std::size_t);
            std::vector<std::pair<std::size_t, it_diagonals>> retrievePossibleNextPoints(std::size_t);
            void markDiagonalProcessed(it_diagonals, std::size_t);

            void logInputData(const std::string &, Logger::CriticalityLevel) const;
            void exportSVG(const std::string &) const;
            void logOutputData(const std::string &, Logger::CriticalityLevel) const;

            const std::vector<Point2<float>> &polygonPoints;
            const std::vector<std::size_t> &endContourIndices; //e.g.: 'polygonPoints' contains 5 CCW points and 4 CW points (hole). So, 'endContourIndices' will have values: 5 and 9.
            const std::vector<std::string> &contourNames;

            std::vector<MonotonePolygon> yMonotonePolygons;
            std::vector<EdgeHelper> edgeHelpers;
            std::multimap<std::size_t, Edge> diagonals;
            std::map<uint_fast64_t, std::set<unsigned int>> sharedEdges;
    };

}

#endif
