#pragma once

#include <string>
#include <vector>
#include <fstream>

#include <math/algebra/point/Point2.h>
#include <io/svg/shape/SVGShape.h>
#include <io/svg/shape/SVGPolygon.h>

namespace urchin {

    class SVGExporter {
        public:
            explicit SVGExporter(std::string);

            void addShape(std::unique_ptr<SVGShape>);

            void generateSVG(int zoomPercentage = 100) const;

        private:
            std::string retrieveViewBox() const;
            void addShapes(std::ofstream&) const;

            std::string filename;
            std::vector<std::unique_ptr<SVGShape>> shapes;
    };

}
