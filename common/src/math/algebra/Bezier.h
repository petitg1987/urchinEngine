#pragma once

#include <vector>

#include <math/algebra/point/Point2.h>

namespace urchin {

    class Bezier {
        public:
            static std::unique_ptr<Bezier> cubic(const Point2<float>&, const Point2<float>&);

            float computeProgression(float) const;

        private:
            explicit Bezier(std::vector<Point2<float>>);

            std::vector<Point2<float>> points;
            mutable std::vector<float> result;
    };

}