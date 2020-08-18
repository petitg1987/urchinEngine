#ifndef URCHINENGINE_CONVEXHULLSHAPE2D_H
#define URCHINENGINE_CONVEXHULLSHAPE2D_H

#include <vector>
#include <memory>

#include "math/geometry/2d/shape/ConvexShape2D.h"
#include "math/algebra/point/Point2.h"

namespace urchin
{

    template<class T> class ConvexHullShape2D : public ConvexShape2D
    {
        public:
            ConvexHullShape2D() = default;
            explicit ConvexHullShape2D(const std::vector<Point2<T>> &);
            ~ConvexHullShape2D() override = default;
            static std::unique_ptr<ConvexHullShape2D<T>> createFromCcwConvexPoints(const std::vector<Point2<T>> &);

            const std::vector<Point2<T>> &getPoints() const;

            Point2<T> getSupportPoint(const Vector2<T> &) const;
            T area() const;

            std::unique_ptr<ConvexHullShape2D<T>> resize(T) const;

        private:
            std::vector<Point2<T>> convexHullPoints;

    };

}

#endif
