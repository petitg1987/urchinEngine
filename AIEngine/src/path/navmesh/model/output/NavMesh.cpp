#include "UrchinCommon.h"

#include "NavMesh.h"

namespace urchin
{

	//static
	unsigned int NavMesh::nextUpdateId = 0;

	NavMesh::NavMesh() :
        updateId(changeUpdateId())
	{

	}

	NavMesh::NavMesh(const NavMesh &navMesh) :
        updateId(changeUpdateId())
	{
        polygons.reserve(navMesh.getPolygons().size());
        for(const std::shared_ptr<NavPolygon> &polygon : navMesh.getPolygons())
        {
            polygons.push_back(std::make_shared<NavPolygon>(*polygon));
        }
	}

	unsigned int NavMesh::getUpdateId() const
	{
		return updateId;
	}

	void NavMesh::replaceAllPolygons(const std::vector<std::shared_ptr<NavPolygon>> &allPolygons)
	{
        changeUpdateId();

	    polygons.clear();
        polygons.insert(polygons.end(), allPolygons.begin(), allPolygons.end());
	}

	const std::vector<std::shared_ptr<NavPolygon>> &NavMesh::getPolygons() const
	{
		return polygons;
	}

	void NavMesh::logNavMesh() const
	{
		std::stringstream logStream;
		logStream.precision(std::numeric_limits<float>::max_digits10);

		logStream<<"Mav mesh:"<<std::endl;

		for(std::size_t i=0; i<getPolygons().size(); ++i)
		{
			const std::shared_ptr<NavPolygon> &polygon = getPolygons()[i];
			logStream<<" - Polygon "<<i<<": "<<polygon->getName()<<std::endl;

			for(std::size_t j=0; j<polygon->getTriangles().size(); ++j)
			{
				const std::shared_ptr<NavTriangle> &triangle = polygon->getTriangle(j);
				logStream<<"  - Triangle "<<j<<": "
                         <<"{"<<triangle->getIndex(0)<<": "<<polygon->getPoint(triangle->getIndex(0))
						 <<"}, {"<<triangle->getIndex(1)<<": "<<polygon->getPoint(triangle->getIndex(1))
						 <<"}, {"<<triangle->getIndex(2)<<": "<<polygon->getPoint(triangle->getIndex(2))<<"}"
						 <<" {"<<triangle->getNeighbor(0)<<", "<<triangle->getNeighbor(1)<<", "<<triangle->getNeighbor(2)<<"}"<<std::endl;
			}
		}

		Logger::logger().log(Logger::INFO, logStream.str());
	}

	void NavMesh::svgMeshExport(const std::string &filename) const
	{
		SVGExporter svgExporter(filename);

		for(const auto &polygon : polygons)
		{
			for(const auto &triangle : polygon->getTriangles())
			{
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

		for(const auto &polygon : polygons)
		{
			for (const auto &triangle : polygon->getTriangles())
			{
				for(std::size_t i=0; i<3; ++i)
				{
					std::shared_ptr<NavTriangle> neighbor = triangle->getNeighbor(i);
					if(neighbor != nullptr)
					{
						Point3<float> lineP1 = triangle->getCenterPoint();
						Point3<float> lineP2 = neighbor->getCenterPoint();
						LineSegment2D<float> line(Point2<float>(lineP1.X, -lineP1.Z), Point2<float>(lineP2.X, -lineP2.Z));

						auto *svgLine = new SVGLine(line, SVGPolygon::BLUE, 0.5f);
						svgLine->setStroke(SVGPolygon::BLUE, 0.05f);
						svgExporter.addShape(svgLine);
					}
				}
			}
		}

        svgExporter.generateSVG(400);
	}

    unsigned int NavMesh::changeUpdateId()
    {
        updateId = ++nextUpdateId;
        return updateId;
    }
}
