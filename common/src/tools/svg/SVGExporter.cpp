#include "SVGExporter.h"

namespace urchin
{
    SVGExporter::SVGExporter(const std::string &filename) :
        filename(filename)
    {

    }

    SVGExporter::~SVGExporter()
    {
        for(const SVGShape *shape : shapes)
        {
           delete shape;
        }
    }

    void SVGExporter::addShape(const SVGShape *shape)
    {
        shapes.push_back(shape);
    }

    void SVGExporter::generateSVG(int zoomPercentage) const
    {
        std::ofstream fileStream;
        fileStream.open(filename);

        fileStream<<"<!DOCTYPE html>"<<std::endl;
        fileStream<<"<html>"<<std::endl;
        fileStream<<"<body>"<<std::endl;
        fileStream<< R"(<svg height=")" + std::to_string(zoomPercentage) + R"(%" width=")" + std::to_string(zoomPercentage) + R"(%" viewbox=")" + retrieveViewBox() + "\">"<<std::endl;

        addShapes(fileStream);

        fileStream<<"</svg>"<<std::endl;
        fileStream<<"</body>"<<std::endl;
        fileStream<<"</html>"<<std::endl;

        fileStream.close();
    }

    std::string SVGExporter::retrieveViewBox() const
    {
        if(shapes.empty())
        {
            return "";
        }

        Rectangle<int> viewBox = shapes[0]->computeRectangle();
        for(const auto shape : shapes)
        {
            viewBox = viewBox.merge(shape->computeRectangle());
        }

        constexpr float MARGIN = 2.0f;
        return std::to_string(viewBox.getMin().X - MARGIN) + " " + std::to_string(viewBox.getMin().Y - MARGIN) + " "
               + std::to_string(viewBox.getMax().X - viewBox.getMin().X + (MARGIN * 2.0f)) + " "
               + std::to_string(viewBox.getMax().Y - viewBox.getMin().Y + (MARGIN * 2.0f));
    }

    void SVGExporter::addShapes(std::ofstream &fileStream) const
    {
        for(const auto &shape : shapes)
        {
            fileStream<<"  "<<shape->getShapeTag()<<std::endl;
        }
    }
}
