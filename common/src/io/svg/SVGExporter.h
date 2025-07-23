#pragma once

#include <string>
#include <vector>

#include "io/svg/shape/SVGShape.h"

namespace urchin {

    class SVGExporter {
        public:
            explicit SVGExporter(std::string);

            void addShape(std::unique_ptr<SVGShape>);

            void generateSVG() const;

        private:
            std::string retrieveViewBox() const;
            void addShapes(std::ofstream&) const;

            std::string filename;
            std::vector<std::unique_ptr<SVGShape>> shapes;
    };

}
