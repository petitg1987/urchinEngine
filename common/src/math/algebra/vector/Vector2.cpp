#include <cassert>
#include <cmath>

#include "math/algebra/vector/Vector2.h"
#include "math/algorithm/MathFunction.h"

namespace urchin {

    template<class T> Vector2<T>::Vector2() noexcept :
            X(0), Y(0) {

    }

    template<class T> Vector2<T>::Vector2(T Xu, T Yu) noexcept :
            X(Xu), Y(Yu) {

    }

    template<class T> void Vector2<T>::setValues(T Xu, T Yu) {
        X = Xu;
        Y = Yu;
    }

    template<class T> void Vector2<T>::setNull() {
        X = Y = 0;
    }

    template<class T> Vector2<T> Vector2<T>::normalize() const noexcept {
        const auto norm = (T)std::sqrt(X * X + Y * Y);
        if (norm > 0.0) [[likely]] {
            T invNorm = (T)1.0 / norm;
            return Vector2<T>(X * invNorm, Y * invNorm);
        }

        return Vector2<T>(X, Y);
    }

    template<class T> T Vector2<T>::length() const noexcept {
        return (T)std::sqrt(X * X + Y * Y);
    }

    template<class T> T Vector2<T>::squareLength() const noexcept {
        return (X * X + Y * Y);
    }

    template<class T> T Vector2<T>::dotProduct(const Vector2<T>& v) const noexcept {
        return (X * v.X + Y * v.Y);
    }

    template<class T> T Vector2<T>::crossProduct(const Vector2<T>& v) const noexcept {
        return (X * v.Y - Y * v.X);
    }

    /**
     * @return Perpendicular vector. There are two possibilities and this method return one of them.
     */
    template<class T> Vector2<T> Vector2<T>::perpendicularVector() const {
        #ifdef URCHIN_DEBUG
            assert(std::is_signed_v<T>);
        #endif

        return Vector2<T>(-Y, X); //second possibility: Vector2<T>(Y, -X)
    }

    template<class T> Vector2<T> Vector2<T>::truncate(T maxLength) const {
        T vLength = length();
        if (vLength < maxLength) {
            return Vector2<T>(X, Y);
        }
        return Vector2<T>(X, Y) * (maxLength / vLength);
    }

    template<class T> Vector2<T> Vector2<T>::operator +() const {
        return Vector2<T>(X, Y);
    }

    template<class T> Vector2<T> Vector2<T>::operator -() const {
        #ifdef URCHIN_DEBUG
            assert(std::is_signed_v<T>);
        #endif

        return Vector2<T>(-X, -Y);
    }

    template<class T> Vector2<T> Vector2<T>::operator +(const Vector2<T>& v) const {
        return Vector2<T>(X + v.X,
                          Y + v.Y);
    }

    template<class T> Vector2<T> Vector2<T>::operator -(const Vector2<T>& v) const {
        return Vector2<T>(X - v.X,
                          Y - v.Y);
    }

    template<class T> Vector2<T> Vector2<T>::operator *(const Vector2<T>& v) const {
        return Vector2<T>(X * v.X,
                          Y * v.Y);
    }

    template<class T> Vector2<T> Vector2<T>::operator /(const Vector2<T>& v) const {
        return Vector2<T>(X / v.X,
                          Y / v.Y);
    }

    template<class T> const Vector2<T>& Vector2<T>::operator +=(const Vector2<T>& v) {
        X += v.X;
        Y += v.Y;

        return *this;
    }

    template<class T> const Vector2<T>& Vector2<T>::operator -=(const Vector2<T>& v) {
        X -= v.X;
        Y -= v.Y;

        return *this;
    }

    template<class T> const Vector2<T>& Vector2<T>::operator *=(const Vector2<T>& v) {
        X *= v.X;
        Y *= v.Y;

        return *this;
    }

    template<class T> const Vector2<T>& Vector2<T>::operator /=(const Vector2<T>& v) {
        X /= v.X;
        Y /= v.Y;

        return *this;
    }

    template<class T> const Vector2<T>& Vector2<T>::operator *=(T t) {
        X *= t;
        Y *= t;

        return *this;
    }

    template<class T> const Vector2<T>& Vector2<T>::operator /=(T t) {
        X /= t;
        Y /= t;

        return *this;
    }

    template<class T> bool Vector2<T>::operator ==(const Vector2<T>& v) const {
        return (X == v.X && Y == v.Y);
    }

    template<class T> std::partial_ordering Vector2<T>::operator <=>(const Vector2<T>& v) const {
        if (auto cmpX = X <=> v.X; cmpX != 0) {
            return cmpX;
        }
        return Y <=> v.Y;
    }

    template<class T> bool Vector2<T>::isEqual(const Vector2<T>& v, T epsilon) const {
        return MathFunction::isEqual(X, v.X, epsilon) && MathFunction::isEqual(Y, v.Y, epsilon);
    }

    template<class T> T& Vector2<T>::operator [](std::size_t i) {
        return (&X)[i];
    }

    template<class T> const T& Vector2<T>::operator [](std::size_t i) const {
        return (&X)[i];
    }

    template<class T> Vector2<T>::operator T*() {
        return &X;
    }

    template<class T> Vector2<T>::operator const T*() const {
        return &X;
    }

    template<class T> template<class NEW_TYPE> Vector2<NEW_TYPE> Vector2<T>::cast() const {
        return Vector2<NEW_TYPE>((NEW_TYPE)X, (NEW_TYPE)Y);
    }

    template<class T> Vector2<T> operator *(const Vector2<T>& v, T t) {
        return Vector2<T>(v.X * t, v.Y * t);
    }

    template<class T> Vector2<T> operator *(T t, const Vector2<T>& v) {
        return v * t;
    }

    template<class T> Vector2<T> operator /(const Vector2<T>& v, T t) {
        return Vector2<T>(v.X / t, v.Y / t);
    }

    template<class T> Vector2<T> operator *(const Matrix2<T>& m, const Vector2<T>& v) {
        return Vector2<T>(m.a11 * v.X + m.a12 * v.Y,
                          m.a21 * v.X + m.a22 * v.Y);
    }

    template<class T> Vector2<T> operator *(const Vector2<T>& v, const Matrix2<T>& m) {
        return m * v;
    }

    template<class T> std::ostream& operator <<(std::ostream& stream, const Vector2<T>& v) {
        return stream << v.X << ", " << v.Y;
    }

    //explicit template
    template class Vector2<float>;
    template Vector2<float> Vector2<float>::cast() const;
    template Vector2<double> Vector2<float>::cast() const;
    template Vector2<int> Vector2<float>::cast() const;
    template Vector2<float> operator *<float>(const Vector2<float>&, float);
    template Vector2<float> operator *<float>(float, const Vector2<float>&);
    template Vector2<float> operator /<float>(const Vector2<float>&, float);
    template Vector2<float> operator *<float>(const Matrix2<float>&, const Vector2<float>&);
    template Vector2<float> operator *<float>(const Vector2<float>&, const Matrix2<float>&);
    template std::ostream& operator <<<float>(std::ostream&, const Vector2<float>&);

    template class Vector2<double>;
    template Vector2<float> Vector2<double>::cast() const;
    template Vector2<double> Vector2<double>::cast() const;
    template Vector2<int> Vector2<double>::cast() const;
    template Vector2<double> operator *<double>(const Vector2<double>&, double);
    template Vector2<double> operator *<double>(double, const Vector2<double>&);
    template Vector2<double> operator /<double>(const Vector2<double>&, double);
    template Vector2<double> operator *<double>(const Matrix2<double>&, const Vector2<double>&);
    template Vector2<double> operator *<double>(const Vector2<double>&, const Matrix2<double>&);
    template std::ostream& operator <<<double>(std::ostream&, const Vector2<double>&);

    template class Vector2<int>;
    template Vector2<float> Vector2<int>::cast() const;
    template Vector2<double> Vector2<int>::cast() const;
    template Vector2<int> Vector2<int>::cast() const;
    template Vector2<int> operator *<int>(const Vector2<int>&, int);
    template Vector2<int> operator *<int>(int, const Vector2<int>&);
    template Vector2<int> operator /<int>(const Vector2<int>&, int);
    template Vector2<int> operator *<int>(const Matrix2<int>&, const Vector2<int>&);
    template Vector2<int> operator *<int>(const Vector2<int>&, const Matrix2<int>&);
    template std::ostream& operator <<<int>(std::ostream&, const Vector2<int>&);

}
