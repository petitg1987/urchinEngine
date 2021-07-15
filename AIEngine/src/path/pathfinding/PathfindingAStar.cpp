#include <algorithm>

#include <path/pathfinding/PathfindingAStar.h>
#include <path/pathfinding/PathPortal.h>
#include <path/pathfinding/FunnelAlgorithm.h>

namespace urchin {

    bool PathNodeCompare::operator()(const std::shared_ptr<PathNode>& node1, const std::shared_ptr<PathNode>& node2) const {
        return node1->getFScore() < node2->getFScore();
    }

    PathfindingAStar::PathfindingAStar(std::shared_ptr<NavMesh> navMesh) :
            jumpAdditionalCost(ConfigService::instance().getFloatValue("pathfinding.jumpAdditionalCost")),
            navMesh(std::move(navMesh)) {

    }

    std::vector<PathPoint> PathfindingAStar::findPath(const Point3<float>& startPoint, const Point3<float>& endPoint) const {
        ScopeProfiler sp(Profiler::ai(), "findPath");

        std::shared_ptr<NavTriangle> startTriangle = findTriangle(startPoint);
        std::shared_ptr<NavTriangle> endTriangle = findTriangle(endPoint);
        if (!startTriangle || !endTriangle) {
            return {}; //no path exists
        }

        float startEndHScore = computeHScore(*startTriangle, endPoint);

        std::set<const NavTriangle*> closedList;
        std::multiset<std::shared_ptr<PathNode>, PathNodeCompare> openList;
        openList.insert(std::make_shared<PathNode>(startTriangle, 0.0, startEndHScore));

        std::shared_ptr<PathNode> endNodePath = nullptr;
        while (!openList.empty()) {
            auto currentNodeIt = openList.begin(); //node with smallest fScore
            std::shared_ptr<PathNode> currentNode = *currentNodeIt;

            closedList.insert(&currentNode->getNavTriangle());
            openList.erase(currentNodeIt);

            const auto& currTriangle = currentNode->getNavTriangle();
            for (const auto& link : currTriangle.getLinks()) {
                auto neighborTriangle = link->getTargetTriangle();

                if (closedList.find(neighborTriangle.get()) != closedList.end()) { //already processed
                    continue;
                }

                std::shared_ptr<PathNode> neighborNodePath = retrievePathNodeFrom(openList, *neighborTriangle);
                if (!neighborNodePath) {
                    float gScore = computeGScore(currentNode, link, startPoint);
                    float hScore = computeHScore(*neighborTriangle, endPoint);
                    neighborNodePath = std::make_shared<PathNode>(neighborTriangle, gScore, hScore);
                    neighborNodePath->setPreviousNode(currentNode, link);

                    if (!endNodePath || neighborNodePath->getFScore() < endNodePath->getFScore()) {
                        openList.insert(neighborNodePath);
                    }

                    if (neighborTriangle.get() == endTriangle.get()) { //end triangle reached but continue on path nodes having a smaller F score
                        endNodePath = neighborNodePath;
                    }
                } else {
                    float gScore = computeGScore(currentNode, link, startPoint);
                    if (neighborNodePath->getGScore() > gScore) { //better path found to reach neighborNodePath: override previous values
                        neighborNodePath->setGScore(gScore);
                        neighborNodePath->setPreviousNode(currentNode, link);
                    }
                }
            }
        }

        if (endNodePath) {
            std::vector<std::unique_ptr<PathPortal>> pathPortals = determinePath(endNodePath, startPoint, endPoint);
            return pathPortalsToPathPoints(pathPortals, true);
        }

        return {}; //no path exists
    }

    std::shared_ptr<NavTriangle> PathfindingAStar::findTriangle(const Point3<float>& point) const {
        float bestVerticalDistance = std::numeric_limits<float>::max();
        std::shared_ptr<NavTriangle> result = nullptr;

        for (const auto& polygon : navMesh->getPolygons()) {
            for (std::size_t triIndex = 0; triIndex < polygon->getTriangles().size(); ++triIndex) {
                const auto& triangle = polygon->getTriangles()[triIndex];
                Point2<float> flattenPoint(point.X, point.Z);

                if (isPointInsideTriangle(flattenPoint, *polygon, *triangle)) {
                    float verticalDistance = point.Y - triangle->getCenterPoint().Y;
                    if (verticalDistance >= 0.0 && verticalDistance < bestVerticalDistance) {
                        bestVerticalDistance = verticalDistance;
                        result = triangle;
                    }
                }
            }
        }
        return result;
    }

    bool PathfindingAStar::isPointInsideTriangle(const Point2<float>& point, const NavPolygon& polygon, const NavTriangle& triangle) const {
        Point2<float> p0 = polygon.getPoint(triangle.getIndex(0)).toPoint2XZ();
        Point2<float> p1 = polygon.getPoint(triangle.getIndex(1)).toPoint2XZ();
        Point2<float> p2 = polygon.getPoint(triangle.getIndex(2)).toPoint2XZ();

        float crossProduct1 = crossProduct(point, p0, p1);
        float crossProduct2 = crossProduct(point, p1, p2);
        if (crossProduct1 * crossProduct2 < 0.0f) { //cross products have different sign
            return false;
        }

        float crossProduct3 = crossProduct(point, p2, p0);
        return crossProduct2 * crossProduct3 >= 0.0f; //check cross products sign
    }

    float PathfindingAStar::crossProduct(const Point2<float>& p1, const Point2<float>& p2, const Point2<float>& p3) const {
        //Same as: p3.vector(p1).crossProduct(p3.vector(p2))
        return (p1.X - p3.X) * (p2.Y - p3.Y) - (p2.X - p3.X) * (p1.Y - p3.Y);
    }

    std::shared_ptr<PathNode> PathfindingAStar::retrievePathNodeFrom(const std::multiset<std::shared_ptr<PathNode>, PathNodeCompare>& pathNodes, const NavTriangle& navTriangle) const {
        for (const auto& pathNode : pathNodes) {
            if (&pathNode->getNavTriangle() == &navTriangle) {
                return pathNode;
            }
        }
        return std::shared_ptr<PathNode>();
    }

    /**
     * Compute score from 'startPoint to 'link'
     */
    float PathfindingAStar::computeGScore(const std::shared_ptr<PathNode>& currentNode, const std::shared_ptr<NavLink>& link, const Point3<float>& startPoint) const {
        std::shared_ptr<PathNode> neighborNodePath = std::make_shared<PathNode>(link->getTargetTriangle(), 0.0f, 0.0f);
        neighborNodePath->setPreviousNode(currentNode, link);
        std::vector<std::unique_ptr<PathPortal>> pathPortals = determinePath(neighborNodePath, startPoint, link->getTargetTriangle()->getCenterPoint());
        std::vector<PathPoint> path = pathPortalsToPathPoints(pathPortals, false);

        float pathCost = 0.0f;
        for (int i = 0; i < (long)path.size() - 1; i++) {
            pathCost += path[(std::size_t)i].getPoint().distance(path[(std::size_t)i + 1].getPoint());
            if (path[(std::size_t)i].isJumpPoint()) {
                pathCost += jumpAdditionalCost;
            }
        }

        return pathCost;
    }

    /**
     * Compute approximate score from 'current' to 'endPoint'
     */
    float PathfindingAStar::computeHScore(const NavTriangle& current, const Point3<float>& endPoint) const {
        Point3<float> currentPoint = current.getCenterPoint();
        return std::abs(currentPoint.X - endPoint.X) + std::abs(currentPoint.Y - endPoint.Y) + std::abs(currentPoint.Z - endPoint.Z);
    }

    std::vector<std::unique_ptr<PathPortal>> PathfindingAStar::determinePath(const std::shared_ptr<PathNode>& endNode, const Point3<float>& startPoint,
                                                                             const Point3<float>& endPoint) const {
        std::vector<std::unique_ptr<PathPortal>> portals;
        portals.reserve(10); //estimated memory size

        std::shared_ptr<PathNode> pathNode = endNode;
        portals.emplace_back(std::make_unique<PathPortal>(LineSegment3D<float>(endPoint, endPoint), pathNode, nullptr, false));
        while (pathNode->getPreviousNode() != nullptr) {
            PathNodeEdgesLink pathNodeEdgesLink = pathNode->computePathNodeEdgesLink();

            LineSegment3D<float> targetPortal = rearrangePortal(pathNodeEdgesLink.targetEdge, portals);
            portals.emplace_back(std::make_unique<PathPortal>(targetPortal, pathNode->getPreviousNode(), pathNode, false));

            if (!pathNodeEdgesLink.areIdenticalEdges) { //source and target edges are different (jump)
                LineSegment3D<float> sourcePortal = rearrangePortal(pathNodeEdgesLink.sourceEdge, portals);
                portals.emplace_back(std::make_unique<PathPortal>(sourcePortal, pathNode->getPreviousNode(), pathNode, true));
            }

            pathNode = pathNode->getPreviousNode();
        }
        portals.emplace_back(std::make_unique<PathPortal>(LineSegment3D<float>(startPoint, startPoint), nullptr, pathNode, false));
        std::reverse(portals.begin(), portals.end());

        FunnelAlgorithm().computePivotPoints(portals);
        return portals;
    }

    /**
     * Rearrange portal in a way first point (getA()) of portal segment must be on left of character when it cross a portal.
     */
    LineSegment3D<float> PathfindingAStar::rearrangePortal(const LineSegment3D<float>& portal, const std::vector<std::unique_ptr<PathPortal>>& portals) const {
        Point3<float> characterPosition = middlePoint(portals.back()->getPortal());
        Vector3<float> characterMoveDirection = characterPosition.vector(middlePoint(portal)).normalize();
        Vector3<float> characterToPortalA = characterPosition.vector(portal.getA()).normalize();
        float crossProductY = characterMoveDirection.Z * characterToPortalA.X - characterMoveDirection.X * characterToPortalA.Z;
        if (crossProductY > 0.0f) {
            return LineSegment3D<float>(portal.getB(), portal.getA());
        }

        return portal;
    }

    Point3<float> PathfindingAStar::middlePoint(const LineSegment3D<float>& lineSegment) const {
        return (lineSegment.getA() + lineSegment.getB()) / 2.0f;
    }

    std::vector<PathPoint> PathfindingAStar::pathPortalsToPathPoints(std::vector<std::unique_ptr<PathPortal>>& pathPortals, bool followTopography) const {
        std::vector<PathPoint> pathPoints;
        pathPoints.reserve(pathPortals.size() / 2); //estimated memory size

        addMissingTransitionPoints(pathPortals);

        for (const auto& pathPortal : pathPortals) {
            if (pathPortal->hasTransitionPoint()) {
                if (followTopography && !pathPoints.empty()) {
                    auto* navPolygonTopography = pathPortal->getPreviousPathNode()->getNavTriangle().getNavPolygon()->getNavTopography();
                    if (navPolygonTopography) {
                        const Point3<float>& startPoint = pathPoints.back().getPoint();
                        const Point3<float>& endPoint = pathPortal->getTransitionPoint();
                        std::vector<Point3<float>> topographyPoints = navPolygonTopography->followTopography(startPoint, endPoint);

                        pathPoints.pop_back();
                        for (std::size_t i = 0; i < topographyPoints.size() - 1; ++i) {
                            pathPoints.emplace_back(PathPoint(topographyPoints[i], false));
                        }
                        pathPoints.emplace_back(PathPoint(topographyPoints.back(), pathPortal->isJumpOriginPortal()));
                    } else {
                        pathPoints.emplace_back(PathPoint(pathPortal->getTransitionPoint(), pathPortal->isJumpOriginPortal()));
                    }
                } else {
                    pathPoints.emplace_back(PathPoint(pathPortal->getTransitionPoint(), pathPortal->isJumpOriginPortal()));
                }
            }
        }

        return pathPoints;
    }

    void PathfindingAStar::addMissingTransitionPoints(std::vector<std::unique_ptr<PathPortal>>& portals) const {
        if (!portals.empty()) {
            assert(portals[0]->hasTransitionPoint());
            assert(portals.back()->hasTransitionPoint());
        }

        Point3<float> previousTransitionPoint = portals[0]->getTransitionPoint();
        for (std::size_t i = 0; i < portals.size(); ++i) {
            if (portals[i]->isJumpOriginPortal()) {
                assert(portals.size() >= i + 1); //jump is composed of two portals (start jump portal & end jump portal)

                Point3<float> jumpStartPoint = computeTransitionPoint(*portals[i], previousTransitionPoint);
                if (!portals[i]->hasTransitionPoint()) {
                    portals[i]->setTransitionPoint(jumpStartPoint);
                }

                i++;
                if (!portals[i]->hasTransitionPoint()) {
                    Point3<float> jumpEndPoint = portals[i]->getPortal().closestPoint(jumpStartPoint);
                    portals[i]->setTransitionPoint(jumpEndPoint);
                }
                previousTransitionPoint = portals[i]->getTransitionPoint();
            } else if (!portals[i]->hasTransitionPoint() && portals[i]->hasDifferentTopography()) {
                portals[i]->setTransitionPoint(computeTransitionPoint(*portals[i], previousTransitionPoint));
                previousTransitionPoint = portals[i]->getTransitionPoint();
            }
        }
    }

    Point3<float> PathfindingAStar::computeTransitionPoint(const PathPortal& portal, const Point3<float>& previousTransitionPoint) const {
        //Compute approximate point for performance reason (note: real point is intersection of [portal->getPortal()] with [nextTransitionPoint.vector(previousTransitionPoint)])
        return portal.getPortal().closestPoint(previousTransitionPoint);
    }

}
