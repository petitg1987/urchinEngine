#include <io/svg/SVGExporter.h>

#include <utility>

namespace urchin {
    SVGExporter::SVGExporter(std::string filename) :
            filename(std::move(filename)) {

    }

    void SVGExporter::addShape(std::unique_ptr<SVGShape> shape) {
        shapes.push_back(std::move(shape));
    }

    void SVGExporter::generateSVG() const {
        std::ofstream fileStream;
        fileStream.open(filename);

        fileStream << R"(<svg height="100%" width="100%" viewbox=")" + retrieveViewBox() + "\">" << std::endl;

        addShapes(fileStream);

        fileStream << "</svg>" << std::endl;

        fileStream.close();
    }

    std::string SVGExporter::retrieveViewBox() const {
        if (shapes.empty()) {
            return "";
        }

        Rectangle<float> viewBox = shapes[0]->computeRectangle();
        for (const auto& shape : shapes) {
            viewBox = viewBox.merge(shape->computeRectangle());
        }

        constexpr float MARGIN_PERCENTAGE = 0.05f;
        float marginX = (viewBox.getMax().X - viewBox.getMin().X) * MARGIN_PERCENTAGE;
        float marginY = (viewBox.getMax().Y - viewBox.getMin().Y) * MARGIN_PERCENTAGE;
        return std::to_string(viewBox.getMin().X - marginX) + " " + std::to_string(viewBox.getMin().Y - marginY) + " "
               + std::to_string(viewBox.getMax().X - viewBox.getMin().X + (marginX * 2.0f)) + " "
               + std::to_string(viewBox.getMax().Y - viewBox.getMin().Y + (marginY * 2.0f));
    }

    void SVGExporter::addShapes(std::ofstream& fileStream) const {
        for (const auto& shape : shapes) {
            fileStream << "  " << shape->getShapeTag() << std::endl;
        }
    }
}
