#include "SVGExporter.h"

namespace urchin
{
    SVGExporter::SVGExporter(const std::string &filename) :
        filename(filename)
    {

    }

    void SVGExporter::addPolygon(const SVGPolygon &polygon)
    {
        polygons.push_back(polygon);
    }

    void SVGExporter::generateSVG(int zoomPercentage) const
    {
        std::ofstream fileStream;
        fileStream.open(filename);

        fileStream<<"<!DOCTYPE html>"<<std::endl;
        fileStream<<"<html>"<<std::endl;
        fileStream<<"<body>"<<std::endl;
        fileStream<< R"(<svg height=")" + std::to_string(zoomPercentage) + R"(%" width=")" + std::to_string(zoomPercentage) + R"(%" viewbox=")" + retrieveViewBox() + "\">"<<std::endl;

        addPolygons(fileStream);

        fileStream<<"</svg>"<<std::endl;
        fileStream<<"</body>"<<std::endl;
        fileStream<<"</html>"<<std::endl;

        fileStream.close();
    }

    std::string SVGExporter::retrieveViewBox() const
    {
        if(polygons.empty())
        {
            return "";
        }

        Rectangle<int> viewBox = polygons[0].computeRectangle();
        for(const auto &polygon : polygons)
        {
            viewBox = viewBox.merge(polygon.computeRectangle());
        }

        constexpr float margin = 2.0f;
        return std::to_string(viewBox.getMin().X - margin) + " " + std::to_string(viewBox.getMin().Y - margin) + " "
               + std::to_string(viewBox.getMax().X - viewBox.getMin().X + margin) + " "
               + std::to_string(viewBox.getMax().Y - viewBox.getMin().Y + margin);
    }

    void SVGExporter::addPolygons(std::ofstream &fileStream) const
    {
        for(const auto &polygon : polygons)
        {
            fileStream<<"  <polygon points=\"";
            for(const auto &polygonPoint : polygon.getPolygonPoints())
            {
                fileStream<<polygonPoint.X<<", "<<polygonPoint.Y<<" ";
            }
            fileStream<<"\" style=\"" + polygon.getStyle() + "\" />"<<std::endl;
        }
    }
}
