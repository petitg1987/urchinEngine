#include <math/algorithm/MinEnclosingSphere.h>

#include <libs/miniball/Seb.h>

namespace urchin {

    template<class T> Sphere<T> MinEnclosingSphere<T>::compute(std::span<Point3<T>> points) {
        std::vector<Seb::Point<double>> mbPoints;
        mbPoints.reserve(points.size());
        for (const auto& point : points) {
            mbPoints.emplace_back(point.X, point.Y, point.Z);
        }

        Seb::Smallest_enclosing_ball<double> miniball(mbPoints);

        Seb::Smallest_enclosing_ball<double>::Coordinate_iterator centerIt = miniball.center_begin();
        return {(T)miniball.radius(), Point3<T>((T)centerIt[0], (T)centerIt[1], (T)centerIt[2])};
    }

    template<class T> template<std::size_t ARRAY_SIZE> Sphere<T> MinEnclosingSphere<T>::compute(const std::array<Point3<T>, ARRAY_SIZE>& points) {
        std::array<Seb::Point<double>, ARRAY_SIZE> mbPoints;
        for (std::size_t i = 0; i < ARRAY_SIZE; ++i) {
            mbPoints[i] = Seb::Point<double>(points[i].X, points[i].Y, points[i].Z);
        }

        Seb::Smallest_enclosing_ball<double> miniball(mbPoints);

        const Seb::Smallest_enclosing_ball<double>::Coordinate_iterator centerIt = miniball.center_begin();
        return {(T)miniball.radius(), Point3<T>((T)centerIt[0], (T)centerIt[1], (T)centerIt[2])};
    }

    //explicit template
    template class MinEnclosingSphere<float>;
    template Sphere<float> MinEnclosingSphere<float>::compute<4ul>(std::array<Point3<float>, 4ul> const&);

    template class MinEnclosingSphere<double>;
    template Sphere<double> MinEnclosingSphere<double>::compute<4ul>(std::array<Point3<double>, 4ul> const&);
}