#include "math/geometry/2d/object/Rectangle.h"

namespace urchin {

    template<class T> Rectangle<T>::Rectangle() :
            rectangleShape(RectangleShape<T>(Vector2<T>(0.0, 0.0))),
            origin(Point2<T>(0.0, 0.0)) {

    }

    /**
     * @param origin Origin of the rectangle (minimum point on X and Y axis)
     */
    template<class T> Rectangle<T>::Rectangle(const Point2<T>& origin, const Vector2<T>& diagonal) :
        rectangleShape(RectangleShape<T>(diagonal)),
        origin(origin) {

    }

    /**
    * @param minPoint Lower point of rectangle
    * @param maxPoint Upper point of rectangle
    */
    template<class T> Rectangle<T>::Rectangle(const Point2<T>& minPoint, const Point2<T>& maxPoint) :
        rectangleShape(RectangleShape<T>(minPoint.vector(maxPoint))),
        origin(minPoint) {

    }

    template<class T> Point2<T> Rectangle<T>::getMin() const {
        return origin;
    }

    template<class T> Point2<T> Rectangle<T>::getMax() const {
        return origin.translate(rectangleShape.getDiagonal());
    }

    template<class T> Vector2<T> Rectangle<T>::getDiagonal() const {
        return rectangleShape.getDiagonal();
    }

    template<class T> Point2<T> Rectangle<T>::getSupportPoint(const Vector2<T>& direction) const {
        const T signX = direction.X < 0.0 ? 0.0 : 1.0;
        const T signY = direction.Y < 0.0 ? 0.0 : 1.0;

        return origin + Point2<T>(signX * rectangleShape.getDiagonal().X, signY * rectangleShape.getDiagonal().Y);
    }

    template<class T> Rectangle<T> Rectangle<T>::merge(const Rectangle<T>& rectangle) const {
        Point2<T> mergedMin(
                getMin().X < rectangle.getMin().X ? getMin().X : rectangle.getMin().X,
                getMin().Y < rectangle.getMin().Y ? getMin().Y : rectangle.getMin().Y
        );

        Point2<T> mergedMax(
                getMax().X > rectangle.getMax().X ? getMax().X : rectangle.getMax().X,
                getMax().Y > rectangle.getMax().Y ? getMax().Y : rectangle.getMax().Y
        );

        return Rectangle<T>(mergedMin, mergedMax);
    }

    template<class T> bool Rectangle<T>::collideWithPoint(const Point2<T>& point) const {
        Point2<T> maxPoint = origin.translate(rectangleShape.getDiagonal());
        return point.X > origin.X && point.X<maxPoint.X && point.Y > origin.Y && point.Y<maxPoint.Y;
    }

    //explicit template
    template class Rectangle<int>;
    template class Rectangle<float>;
}
