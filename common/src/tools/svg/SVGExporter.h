#ifndef URCHINENGINE_SVGEXPORTER_H
#define URCHINENGINE_SVGEXPORTER_H

#include <string>
#include <vector>
#include <fstream>

#include "math/algebra/point/Point2.h"
#include "tools/svg/shape/SVGShape.h"
#include "tools/svg/shape/SVGPolygon.h"

namespace urchin {

    class SVGExporter {
        public:
            explicit SVGExporter(std::string);
            ~SVGExporter();

            void addShape(const SVGShape*);

            void generateSVG(int zoomPercentage = 100) const;

        private:
            std::string retrieveViewBox() const;
            void addShapes(std::ofstream&) const;

            std::string filename;
            std::vector<const SVGShape*> shapes;
    };

}

#endif
