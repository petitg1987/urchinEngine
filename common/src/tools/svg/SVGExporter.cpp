#include "SVGExporter.h"

#include <utility>

namespace urchin {
    SVGExporter::SVGExporter(std::string filename) :
        filename(std::move(filename)) {

    }

    SVGExporter::~SVGExporter() {
        for(const SVGShape *shape : shapes) {
           delete shape;
        }
    }

    void SVGExporter::addShape(const SVGShape *shape) {
        shapes.push_back(shape);
    }

    void SVGExporter::generateSVG(int zoomPercentage) const {
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

    std::string SVGExporter::retrieveViewBox() const {
        if(shapes.empty()) {
            return "";
        }

        Rectangle<float> viewBox = shapes[0]->computeRectangle();
        for(const auto shape : shapes) {
            viewBox = viewBox.merge(shape->computeRectangle());
        }

        constexpr float MARGIN_PERCENTAGE = 0.05f;
        float marginX = (viewBox.getMax().X - viewBox.getMin().X) * MARGIN_PERCENTAGE;
        float marginY = (viewBox.getMax().Y - viewBox.getMin().Y) * MARGIN_PERCENTAGE;
        return std::to_string(viewBox.getMin().X - marginX) + " " + std::to_string(viewBox.getMin().Y - marginY) + " "
               + std::to_string(viewBox.getMax().X - viewBox.getMin().X + (marginX * 2.0f)) + " "
               + std::to_string(viewBox.getMax().Y - viewBox.getMin().Y + (marginY * 2.0f));
    }

    void SVGExporter::addShapes(std::ofstream &fileStream) const {
        for(const auto &shape : shapes) {
            fileStream<<"  "<<shape->getShapeTag()<<std::endl;
        }
    }
}
