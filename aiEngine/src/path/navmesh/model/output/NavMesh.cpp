#include <UrchinCommon.h>

#include <path/navmesh/model/output/NavMesh.h>
#include <path/navmesh/model/NavModelCopy.h>

namespace urchin {

    //static
    unsigned int NavMesh::nextUpdateId = 0;

    NavMesh::NavMesh() :
            updateId(0) {

    }

    NavMesh::NavMesh(const NavMesh& navMesh) :
            updateId(navMesh.getUpdateId()) {
        NavModelCopy::copyNavPolygons(navMesh.getPolygons(), polygons);
    }

    unsigned int NavMesh::getUpdateId() const {
        return updateId;
    }

    void NavMesh::copyAllPolygons(const std::vector<std::shared_ptr<NavPolygon>>& allPolygons) {
        changeUpdateId();

        polygons.clear();
        NavModelCopy::copyNavPolygons(allPolygons, polygons);
    }

    const std::vector<std::shared_ptr<NavPolygon>>& NavMesh::getPolygons() const {
        return polygons;
    }

    void NavMesh::svgMeshExport(std::string filename) const {
        SVGExporter svgExporter(std::move(filename));

        for (const auto& polygon : polygons) {
            for (const auto& triangle : polygon->getTriangles()) {
                std::vector<Point2<float>> trianglePoints;
                Point3<float> p1 = polygon->getPoint(triangle->getIndices()[0]);
                Point3<float> p2 = polygon->getPoint(triangle->getIndices()[1]);
                Point3<float> p3 = polygon->getPoint(triangle->getIndices()[2]);

                trianglePoints.emplace_back(p1.X, p1.Z);
                trianglePoints.emplace_back(p2.X, p2.Z);
                trianglePoints.emplace_back(p3.X, p3.Z);

                auto svgPolygon = std::make_unique<SVGPolygon>(trianglePoints, SVGColor::LIME, 0.5f);
                svgPolygon->setStroke(SVGColor::RED, 0.05f);
                svgExporter.addShape(std::move(svgPolygon));
            }
        }

        for (const auto& polygon : polygons) {
            for (const auto& triangle : polygon->getTriangles()) {
                for (const auto& link : triangle->getLinks()) {
                    Point3<float> lineP1 = triangle->getCenterPoint();
                    Point3<float> lineP2 = link->getTargetTriangle()->getCenterPoint();
                    LineSegment2D line(Point2<float>(lineP1.X, -lineP1.Z), Point2<float>(lineP2.X, -lineP2.Z));

                    SVGColor linkColor = (link->getLinkType() == NavLinkType::JUMP) ? SVGColor::ORANGE : SVGColor::BLUE;
                    auto svgLine = std::make_unique<SVGLine>(line, linkColor, 0.5f);
                    svgLine->setStroke(linkColor, 0.05f);
                    svgExporter.addShape(std::move(svgLine));
                }
            }
        }

        svgExporter.generateSVG();
    }

    unsigned int NavMesh::changeUpdateId() {
        updateId = ++nextUpdateId;
        return updateId;
    }
}
