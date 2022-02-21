#include <utility>

#include <math/algebra/Bezier.h>

namespace urchin {

    Bezier::Bezier(std::vector<Point2<float>> points) :
            points(std::move(points)) {
        result.resize(this->points.size());
    }

    /**
     * See https://www.cssportal.com/css-cubic-bezier-generator/ to generate curves
     */
    std::unique_ptr<Bezier> Bezier::cubic(const Point2<float>& secondPoints, const Point2<float>& thirdPoint) {
        return std::unique_ptr<Bezier>(new Bezier({Point2<float>(0.0f, 0.0f), secondPoints, thirdPoint, Point2<float>(1.0f, 1.0f)}));
    }

    float Bezier::computeProgression(float t) const {
        for (std::size_t i = 0; i < points.size(); ++i) {
            result[i] = points[i].Y;
        }

        std::size_t i = result.size() - 1;
        while (i > 0) {
            for (std::size_t k = 0; k < i; k++) {
                result[k] = result[k] + t * (result[k + 1] - result[k]);
            }
            i--;
        }
        return result[0];
    }

}