#include <math/algebra/vector/Vector3.h>

namespace urchin {

    template<class T> Vector3<T>::Vector3() :
            X(0), Y(0), Z(0) {

    }

    template<class T> Vector3<T>::Vector3(T Xu, T Yu, T Zu) :
            X(Xu), Y(Yu), Z(Zu) {

    }

    template<class T> Vector3<T>::Vector3(const Vector2<T>& v, T Zu) :
            X(v.X), Y(v.Y), Z(Zu) {

    }

    template<class T> void Vector3<T>::setValues(T Xu, T Yu, T Zu) {
        X = Xu;
        Y = Yu;
        Z = Zu;
    }

    template<class T> void Vector3<T>::setNull() {
        X = Y = Z = 0;
    }

    template<class T> Vector3<T> Vector3<T>::normalize() const {
        const auto norm = (T)std::sqrt(X * X + Y * Y + Z * Z);
        if (norm > 0.0) [[likely]] {
            return Vector3<T>(X / norm, Y / norm, Z / norm);
        }

        return Vector3<T>(X, Y, Z);
    }

    template<class T> T Vector3<T>::length() const {
        return (T)std::sqrt(X * X + Y * Y + Z * Z);
    }

    template<class T> T Vector3<T>::squareLength() const {
        return (X * X + Y * Y + Z * Z);
    }

    template<class T> T Vector3<T>::dotProduct(const Vector3<T>& v) const {
        return (X * v.X + Y * v.Y + Z * v.Z);
    }

    template<class T> Vector3<T> Vector3<T>::crossProduct(const Vector3<T>& v) const {
        return Vector3<T>(Y * v.Z - Z * v.Y,
                          Z * v.X - X * v.Z,
                          X * v.Y - Y * v.X);
    }

    /**
     * @return Perpendicular vector. There are an infinite number of possibilities and this method return one of them.
     */
    template<class T> Vector3<T> Vector3<T>::perpendicularVector() const {
        Vector3<T> perpendicularVector = this->crossProduct(Vector3<T>(1.0, 0.0, 0.0));
        if (perpendicularVector.squareLength() <= std::numeric_limits<T>::epsilon()) {
            perpendicularVector = this->crossProduct(Vector3<T>(0.0, 1.0, 0.0));
        }
        return perpendicularVector;
    }

    template<class T> Vector3<T> Vector3<T>::truncate(T maxLength) const {
        T vLength = length();
        if (vLength < maxLength) {
            return Vector3<T>(X, Y, Z);
        }
        return Vector3<T>(X, Y, Z) * (maxLength / vLength);
    }

    template<class T> Vector2<T> Vector3<T>::xy() const {
        return Vector2<T>(X, Y);
    }

    template<class T> Vector2<T> Vector3<T>::xz() const {
        return Vector2<T>(X, Z);
    }

    template<class T> Vector2<T> Vector3<T>::yz() const {
        return Vector2<T>(Y, Z);
    }

    template<class T> Vector3<T> Vector3<T>::operator +() const {
        return Vector3<T>(X, Y, Z);
    }

    template<class T> Vector3<T> Vector3<T>::operator -() const {
        return Vector3<T>(-X, -Y, -Z);
    }

    template<class T> Vector3<T> Vector3<T>::operator +(const Vector3<T>& v) const {
        return Vector3<T>(    X + v.X,
                              Y + v.Y,
                              Z + v.Z);
    }

    template<class T> Vector3<T> Vector3<T>::operator -(const Vector3<T>& v) const {
        return Vector3<T>(X - v.X,
                          Y - v.Y,
                          Z - v.Z);
    }

    template<class T> Vector3<T> Vector3<T>::operator *(const Vector3<T>& v) const {
        return Vector3<T>(X * v.X,
                          Y * v.Y,
                          Z * v.Z);
    }

    template<class T> Vector3<T> Vector3<T>::operator /(const Vector3<T>& v) const {
        return Vector3<T>(X / v.X,
                          Y / v.Y,
                          Z / v.Z);
    }

    template<class T> const Vector3<T>& Vector3<T>::operator +=(const Vector3<T>& v) {
        X += v.X;
        Y += v.Y;
        Z += v.Z;

        return *this;
    }

    template<class T> const Vector3<T>& Vector3<T>::operator -=(const Vector3<T>& v) {
        X -= v.X;
        Y -= v.Y;
        Z -= v.Z;

        return *this;
    }

    template<class T> const Vector3<T>& Vector3<T>::operator *=(const Vector3<T>& v) {
        X *= v.X;
        Y *= v.Y;
        Z *= v.Z;

        return *this;
    }

    template<class T> const Vector3<T>& Vector3<T>::operator /=(const Vector3<T>& v) {
        X /= v.X;
        Y /= v.Y;
        Z /= v.Z;

        return *this;
    }

    template<class T> const Vector3<T>& Vector3<T>::operator *=(T t) {
        X *= t;
        Y *= t;
        Z *= t;

        return *this;
    }

    template<class T> const Vector3<T>& Vector3<T>::operator /=(T t) {
        X /= t;
        Y /= t;
        Z /= t;

        return *this;
    }

    template<class T> bool Vector3<T>::operator ==(const Vector3<T>& p) const {
        return (X == p.X && Y == p.Y && Z == p.Z);
    }

    template<class T> std::partial_ordering Vector3<T>::operator <=>(const Vector3<T>& p) const {
        if (auto cmpX = X <=> p.X; cmpX != 0) {
            return cmpX;
        } else if (auto cmpY = Y <=> p.Y; cmpY != 0) {
            return cmpY;
        }
        return Z <=> p.Z;
    }

    template<class T> T& Vector3<T>::operator [](std::size_t i) {
        return (&X)[i];
    }

    template<class T> const T& Vector3<T>::operator [](std::size_t i) const {
        return (&X)[i];
    }

    template<class T> Vector3<T>::operator T*() {
        return &X;
    }

    template<class T> Vector3<T>::operator const T*() const {
        return &X;
    }

    template<class T> template<class NEW_TYPE> Vector3<NEW_TYPE> Vector3<T>::cast() const {
        return Vector3<NEW_TYPE>((NEW_TYPE)X, (NEW_TYPE)Y, (NEW_TYPE)Z);
    }

    template<class T> Vector3<T> operator *(const Vector3<T>& v, T t) {
        return Vector3<T>(v.X * t, v.Y * t, v.Z * t);
    }

    template<class T> Vector3<T> operator *(T t, const Vector3<T>& v) {
        return v * t;
    }

    template<class T> Vector3<T> operator /(const Vector3<T>& v, T t) {
        return Vector3<T>(v.X / t, v.Y / t, v.Z / t);
    }

    template<class T> Vector3<T> operator *(const Matrix3<T>& m, const Vector3<T>& v) {
        return Vector3<T>(m.a11 * v.X + m.a12 * v.Y + m.a13 * v.Z,
                          m.a21 * v.X + m.a22 * v.Y + m.a23 * v.Z,
                          m.a31 * v.X + m.a32 * v.Y + m.a33 * v.Z);
    }

    template<class T> Vector3<T> operator *(const Vector3<T>& v, const Matrix3<T>& m) {
        return m * v;
    }

    template<class T> std::ostream& operator <<(std::ostream& stream, const Vector3<T>& v) {
        return stream << v.X << ", " << v.Y << ", " << v.Z;
    }

    //explicit template
    template class Vector3<float>;
    template Vector3<float> Vector3<float>::cast() const;
    template Vector3<double> Vector3<float>::cast() const;
    template Vector3<int> Vector3<float>::cast() const;
    template Vector3<float> operator *<float>(const Vector3<float>&, float);
    template Vector3<float> operator *<float>(float, const Vector3<float>&);
    template Vector3<float> operator /<float>(const Vector3<float>&, float);
    template Vector3<float> operator *<float>(const Matrix3<float>&, const Vector3<float>&);
    template Vector3<float> operator *<float>(const Vector3<float>&, const Matrix3<float>&);
    template std::ostream& operator <<<float>(std::ostream&, const Vector3<float>&);

    template class Vector3<double>;
    template Vector3<float> Vector3<double>::cast() const;
    template Vector3<double> Vector3<double>::cast() const;
    template Vector3<int> Vector3<double>::cast() const;
    template Vector3<double> operator *<double>(const Vector3<double>&, double);
    template Vector3<double> operator *<double>(double, const Vector3<double>&);
    template Vector3<double> operator /<double>(const Vector3<double>&, double);
    template Vector3<double> operator *<double>(const Matrix3<double>&, const Vector3<double>&);
    template Vector3<double> operator *<double>(const Vector3<double>&, const Matrix3<double>&);
    template std::ostream& operator <<<double>(std::ostream&, const Vector3<double>&);

    template class Vector3<int>;
    template Vector3<float> Vector3<int>::cast() const;
    template Vector3<double> Vector3<int>::cast() const;
    template Vector3<int> Vector3<int>::cast() const;
    template Vector3<int> operator *<int>(const Vector3<int>&, int);
    template Vector3<int> operator *<int>(int, const Vector3<int>&);
    template Vector3<int> operator /<int>(const Vector3<int>&, int);
    template Vector3<int> operator *<int>(const Matrix3<int>&, const Vector3<int>&);
    template Vector3<int> operator *<int>(const Vector3<int>&, const Matrix3<int>&);
    template std::ostream& operator <<<int>(std::ostream&, const Vector3<int>&);

}
