#include "UrchinCommon.h"

#include "NavMesh.h"
#include "path/navmesh/model/NavModelCopy.h"

namespace urchin {

    //static
    unsigned int NavMesh::nextUpdateId = 0;

    NavMesh::NavMesh() :
        updateId(0) {

    }

    NavMesh::NavMesh(const NavMesh &navMesh) :
        updateId(navMesh.getUpdateId()) {
        NavModelCopy::copyNavPolygons(navMesh.getPolygons(), polygons);
    }

    unsigned int NavMesh::getUpdateId() const {
        return updateId;
    }

    void NavMesh::copyAllPolygons(const std::vector<std::shared_ptr<NavPolygon>> &allPolygons) {
        changeUpdateId();

        polygons.clear();
        NavModelCopy::copyNavPolygons(allPolygons, polygons);
    }

    const std::vector<std::shared_ptr<NavPolygon>> &NavMesh::getPolygons() const {
        return polygons;
    }

    void NavMesh::svgMeshExport(const std::string &filename) const {
        SVGExporter svgExporter(filename);

        for (const auto &polygon : polygons) {
            for (const auto &triangle : polygon->getTriangles()) {
                std::vector<Point2<float>> trianglePoints;
                Point3<float> p1 = polygon->getPoint(triangle->getIndices()[0]);
                Point3<float> p2 = polygon->getPoint(triangle->getIndices()[1]);
                Point3<float> p3 = polygon->getPoint(triangle->getIndices()[2]);

                trianglePoints.emplace_back(Point2<float>(p1.X, -p1.Z));
                trianglePoints.emplace_back(Point2<float>(p2.X, -p2.Z));
                trianglePoints.emplace_back(Point2<float>(p3.X, -p3.Z));

                auto *svgPolygon = new SVGPolygon(trianglePoints, SVGPolygon::LIME, 0.5f);
                svgPolygon->setStroke(SVGPolygon::RED, 0.05f);
                svgExporter.addShape(svgPolygon);
            }
        }

        for (const auto &polygon : polygons) {
            for (const auto &triangle : polygon->getTriangles()) {
                for (const auto &link : triangle->getLinks()) {
                    Point3<float> lineP1 = triangle->getCenterPoint();
                    Point3<float> lineP2 = link->getTargetTriangle()->getCenterPoint();
                    LineSegment2D<float> line(Point2<float>(lineP1.X, -lineP1.Z), Point2<float>(lineP2.X, -lineP2.Z));

                    auto *svgLine = new SVGLine(line, SVGPolygon::BLUE, 0.5f);
                    svgLine->setStroke(SVGPolygon::BLUE, 0.05f);
                    svgExporter.addShape(svgLine);
                }
            }
        }

        svgExporter.generateSVG(400);
    }

    unsigned int NavMesh::changeUpdateId() {
        updateId = ++nextUpdateId;
        return updateId;
    }
}
