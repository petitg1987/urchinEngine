#include "math/algebra/point/Point3.h"

namespace urchin {

    template<class T> Point3<T>::Point3() :
        X(0), Y(0), Z(0) {

    }

    template<class T> Point3<T>::Point3(T Xu, T Yu, T Zu) :
        X(Xu), Y(Yu), Z(Zu) {

    }

    template<class T> Point3<T>::Point3(const Point2<T> &p, T Zu) {
        X = p.X;
        Y = p.Y;
        Z = Zu;
    }

    template<class T> Point3<T>::Point3(const Vector2<T> &vector, T Zu) :
        X(vector.X), Y(vector.Y), Z(Zu) {

    }

    template<class T> Point3<T>::Point3(const Vector3<T> &vector) :
        X(vector.X), Y(vector.Y), Z(vector.Z) {

    }

    template<class T> Point3<T>::Point3(const Point3<T> &point) :
        X(point.X), Y(point.Y), Z(point.Z) {

    }

    template<class T> Point3<T>& Point3<T>::operator=(const Point3<T> &point) {
        X = point.X;
        Y = point.Y;
        Z = point.Z;
        return *this;
    }

    template<class T> void Point3<T>::setValues(T Xu, T Yu, T Zu) {
        X = Xu;
        Y = Yu;
        Z = Zu;
    }

    template<class T> void Point3<T>::setNull() {
        X = Y = Z = 0;
    }

    template<class T> T Point3<T>::distance(const Point3<T> &p) const {
        return std::sqrt(((X-p.X)*(X-p.X)) + ((Y-p.Y)*(Y-p.Y)) + ((Z-p.Z)*(Z-p.Z)));
    }

    template<class T> T Point3<T>::squareDistance(const Point3<T> &p) const {
        return (T)(((X-p.X)*(X-p.X)) + ((Y-p.Y)*(Y-p.Y)) + ((Z-p.Z)*(Z-p.Z)));
    }

    template<class T> Vector3<T> Point3<T>::vector(const Point3<T> &target) const {
        return Vector3<T>(target.X - X, target.Y - Y, target.Z - Z);
    }

    /**
     * @return Vector from origin (0.0, 0.0, 0.0) to this point
     */
    template<class T> Vector3<T> Point3<T>::toVector() const {
        return Vector3<T>(X, Y, Z);
    }

    template<class T> Point3<T> Point3<T>::translate(const Vector3<T> &v) const {
        return Point3<T>(X+v.X, Y+v.Y, Z+v.Z);
    }

    template<class T> Point3<T> Point3<T>::divideByZ() const {
        if (Z!=0.0) {
            return Point3<T>(X/Z, Y/Z, 1);
        }

        return *this;
    }

    template<class T> Point2<T> Point3<T>::toPoint2XY() const {
        return Point2<T>(X, Y);
    }

    template<class T> Point2<T> Point3<T>::toPoint2XZ() const {
        return Point2<T>(X, Z);
    }

    template<class T> Point2<T> Point3<T>::toPoint2YZ() const {
        return Point2<T>(Y, Z);
    }

    template<class T> Point3<T> Point3<T>::operator +() const {
        return Point3<T>(X, Y, Z);
    }

    template<class T> Point3<T> Point3<T>::operator -() const {
        return Point3<T>(-X, -Y, -Z);
    }

    template<class T> Point3<T> Point3<T>::operator +(const Point3<T> &p) const {
        return Point3<T>(    X + p.X,
                    Y + p.Y,
                    Z + p.Z);
    }

    template<class T> Point3<T> Point3<T>::operator -(const Point3<T> &p) const {
        return Point3<T>(    X - p.X,
                    Y - p.Y,
                    Z - p.Z);
    }

    template<class T> const Point3<T>& Point3<T>::operator +=(const Point3<T> &p) {
        X += p.X;
        Y += p.Y;
        Z += p.Z;

        return *this;
    }

    template<class T> const Point3<T>& Point3<T>::operator -=(const Point3<T> &p) {
        X -= p.X;
        Y -= p.Y;
        Z -= p.Z;

        return *this;
    }

    template<class T> const Point3<T>& Point3<T>::operator *=(T t) {
        X *= t;
        Y *= t;
        Z *= t;

        return *this;
    }

    template<class T> const Point3<T>& Point3<T>::operator /=(T t) {
        X /= t;
        Y /= t;
        Z /= t;

        return *this;
    }

    template<class T> const Point3<T>& Point3<T>::operator +=(T t) {
        X += t;
        Y += t;
        Z += t;

        return *this;
    }

    template<class T> const Point3<T>& Point3<T>::operator -=(T t) {
        X -= t;
        Y -= t;
        Z -= t;

        return *this;
    }

    template<class T> bool Point3<T>::operator ==(const Point3<T> &p) const {
        return (X==p.X && Y==p.Y && Z==p.Z);
    }

    template<class T> bool Point3<T>::operator !=(const Point3<T> &p) const {
        return !(this->operator==(p));
    }

    template<class T> bool Point3<T>::operator <(const Point3<T> &p) const {
        return X < p.X || (X == p.X && Y < p.Y) || (X == p.X && Y == p.Y && Z < p.Z);
    }

    template<class T> T& Point3<T>::operator [](std::size_t i) {
        return (&X)[i];
    }

    template<class T> const T& Point3<T>::operator [](std::size_t i) const {
        return (&X)[i];
    }

    template<class T> Point3<T>::operator T*() {
        return &X;
    }

    template<class T> Point3<T>::operator const T*() const {
        return &X;
    }

    template<class T> template<class NEW_TYPE> Point3<NEW_TYPE> Point3<T>::cast() const {
        return Point3<NEW_TYPE>((NEW_TYPE)X, (NEW_TYPE)Y, (NEW_TYPE)Z);
    }

    template<class T> Point3<T> operator *(const Point3<T> &p, T t) {
        return Point3<T>(p.X * t, p.Y * t, p.Z * t);
    }

    template<class T> Point3<T> operator *(T t, const Point3<T> &p) {
        return p * t;
    }

    template<class T> Point3<T> operator /(const Point3<T> &p, T t) {
        return Point3<T>(p.X / t, p.Y / t, p.Z / t);
    }

    template<class T> Point3<T> operator +(const Point3<T> &p, T t) {
        return Point3<T>(p.X + t, p.Y + t, p.Z + t);
    }

    template<class T> Point3<T> operator -(const Point3<T> &p, T t) {
        return Point3<T>(p.X - t, p.Y - t, p.Z - t);
    }

    template<class T> Point3<T> operator *(const Matrix3<T> &m, const Point3<T> &p) {
        return Point3<T>(    m.a11 * p.X + m.a12 * p.Y + m.a13 * p.Z,
                    m.a21 * p.X + m.a22 * p.Y + m.a23 * p.Z,
                    m.a31 * p.X + m.a32 * p.Y + m.a33 * p.Z);
    }

    template<class T> Point3<T> operator *(const Point3<T> &p, const Matrix3<T> &m) {
        return m * p;
    }

    template<class T> std::ostream& operator <<(std::ostream &stream, const Point3<T> &p) {
        return stream << p.X << ", " << p.Y << ", " << p.Z;
    }

    //explicit template
    template class Point3<float>;
    template Point3<float> Point3<float>::cast() const;
    template Point3<double> Point3<float>::cast() const;
    template Point3<float> operator *<float>(const Point3<float> &, float);
    template Point3<float> operator *<float>(float t, const Point3<float> &);
    template Point3<float> operator /<float>(const Point3<float> &, float);
    template Point3<float> operator +<float>(const Point3<float> &, float);
    template Point3<float> operator -<float>(const Point3<float> &, float);
    template Point3<float> operator *<float>(const Matrix3<float> &, const Point3<float> &);
    template Point3<float> operator *<float>(const Point3<float> &, const Matrix3<float> &);
    template std::ostream& operator <<<float>(std::ostream &, const Point3<float> &);

    template class Point3<double>;
    template Point3<float> Point3<double>::cast() const;
    template Point3<double> Point3<double>::cast() const;
    template Point3<double> operator *<double>(const Point3<double> &, double);
    template Point3<double> operator *<double>(double t, const Point3<double> &);
    template Point3<double> operator /<double>(const Point3<double> &, double);
    template Point3<double> operator +<double>(const Point3<double> &, double);
    template Point3<double> operator -<double>(const Point3<double> &, double);
    template Point3<double> operator *<double>(const Matrix3<double> &, const Point3<double> &);
    template Point3<double> operator *<double>(const Point3<double> &, const Matrix3<double> &);
    template std::ostream& operator <<<double>(std::ostream &, const Point3<double> &);

}
