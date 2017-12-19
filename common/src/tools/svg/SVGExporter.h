#ifndef TESTENGINESFML_SVGEXPORTER_H
#define TESTENGINESFML_SVGEXPORTER_H

#include <string>
#include <vector>
#include <fstream>

#include "math/algebra/point/Point2.h"
#include "tools/svg/SVGPolygon.h"

namespace urchin
{

    class SVGExporter
    {
        public:
            explicit SVGExporter(const std::string &);

            void addPolygon(const SVGPolygon &);

            void generateSVG(int zoomPercentage = 100) const;

        private:
            std::string retrieveViewBox() const;
            void addPolygons(std::ofstream &fileStream) const;

            std::string filename;
            std::vector<SVGPolygon> polygons;
    };

}

#endif
