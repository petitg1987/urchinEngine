#include <math/algebra/vector/Vector4.h>

namespace urchin {

    template<class T> Vector4<T>::Vector4() :
            X(0), Y(0), Z(0), W(1) {

    }

    template<class T> Vector4<T>::Vector4(T Xu, T Yu, T Zu, T Wu) :
            X(Xu), Y(Yu), Z(Zu), W(Wu) {

    }

    template<class T> Vector4<T>::Vector4(const Vector3<T>& v, T Wu) :
            X(v.X), Y(v.Y), Z(v.Z), W(Wu) {

    }

    template<class T> void Vector4<T>::setValues(T Xu, T Yu, T Zu, T Wu) {
        X = Xu;
        Y = Yu;
        Z = Zu;
        W = Wu;
    }

    template<class T> void Vector4<T>::setNull() {
        X = Y = Z = W = 0;
    }

    template<class T> Vector4<T> Vector4<T>::normalize() const {
        const auto norm = (T)std::sqrt(X * X + Y * Y + Z * Z + W * W);

        if (norm > 0.0) [[likely]] {
            return Vector4<T>(X / norm, Y / norm, Z / norm, W / norm);
        }

        return Vector4<T>(X, Y, Z, W);
    }

    template<class T> T Vector4<T>::length() const {
        return (T)std::sqrt(X * X + Y * Y + Z * Z + W * W);
    }

    template<class T> T Vector4<T>::squareLength() const {
        return (X * X + Y * Y + Z * Z + W * W);
    }

    template<class T> T Vector4<T>::dotProduct(const Vector4<T>& v) const {
        return (X * v.X + Y * v.Y + Z * v.Z + W * v.W);
    }

    template<class T> Vector3<T> Vector4<T>::xyz() const {
        return Vector3<T>(X, Y, Z);
    }

    template<class T> Vector4<T> Vector4<T>::operator +() const {
        return Vector4<T>(X, Y, Z, W);
    }

    template<class T> Vector4<T> Vector4<T>::operator -() const {
        return Vector4<T>(-X, -Y, -Z, -W);
    }

    template<class T> Vector4<T> Vector4<T>::operator +(const Vector4<T>& v) const {
        return Vector4<T>(    X + v.X,
                    Y + v.Y,
                    Z + v.Z,
                    W + v.W);
    }

    template<class T> Vector4<T> Vector4<T>::operator -(const Vector4<T>& v) const {
        return Vector4<T>(X - v.X,
                          Y - v.Y,
                          Z - v.Z,
                          W - v.W);
    }

    template<class T> Vector4<T> Vector4<T>::operator *(const Vector4<T>& v) const {
        return Vector4<T>(X * v.X,
                          Y * v.Y,
                          Z * v.Z,
                          W * v.W);
    }

    template<class T> Vector4<T> Vector4<T>::operator /(const Vector4<T>& v) const {
        return Vector4<T>(X / v.X,
                          Y / v.Y,
                          Z / v.Z,
                          W / v.W);
    }

    template<class T> const Vector4<T>& Vector4<T>::operator +=(const Vector4<T>& v) {
        X += v.X;
        Y += v.Y;
        Z += v.Z;
        W += v.W;

        return *this;
    }

    template<class T> const Vector4<T>& Vector4<T>::operator -=(const Vector4<T>& v) {
        X -= v.X;
        Y -= v.Y;
        Z -= v.Z;
        W -= v.W;

        return *this;
    }

    template<class T> const Vector4<T>& Vector4<T>::operator *=(const Vector4<T>& v) {
        X *= v.X;
        Y *= v.Y;
        Z *= v.Z;
        W *= v.W;

        return *this;
    }

    template<class T> const Vector4<T>& Vector4<T>::operator /=(const Vector4<T>& v) {
        X /= v.X;
        Y /= v.Y;
        Z /= v.Z;
        W /= v.W;

        return *this;
    }

    template<class T> const Vector4<T>& Vector4<T>::operator *=(T t) {
        X *= t;
        Y *= t;
        Z *= t;
        W *= t;

        return *this;
    }

    template<class T> const Vector4<T>& Vector4<T>::operator /=(T t) {
        X /= t;
        Y /= t;
        Z /= t;
        W /= t;

        return *this;
    }

    template<class T> bool Vector4<T>::operator ==(const Vector4<T>& p) const {
        return (X == p.X && Y == p.Y && Z == p.Z && W == p.W);
    }

    template<class T> std::partial_ordering Vector4<T>::operator <=>(const Vector4<T>& p) const {
        if (auto cmpX = X <=> p.X; cmpX != 0) {
            return cmpX;
        } else if (auto cmpY = Y <=> p.Y; cmpY != 0) {
            return cmpY;
        } else if (auto cmpZ = Z <=> p.Z; cmpZ != 0) {
            return cmpZ;
        }
        return W <=> p.W;
    }

    template<class T> T& Vector4<T>::operator [](std::size_t i) {
        return (&X)[i];
    }

    template<class T> const T& Vector4<T>::operator [](std::size_t i) const {
        return (&X)[i];
    }

    template<class T> Vector4<T>::operator T*() {
        return &X;
    }

    template<class T> Vector4<T>::operator const T*() const {
        return &X;
    }

    template<class T> template<class NEW_TYPE> Vector4<NEW_TYPE> Vector4<T>::cast() const {
        return Vector4<NEW_TYPE>((NEW_TYPE)X, (NEW_TYPE)Y, (NEW_TYPE)Z, (NEW_TYPE)W);
    }

    template<class T> Vector4<T> operator *(const Vector4<T>& v, T t) {
        return Vector4<T>(v.X * t, v.Y * t, v.Z * t, v.W * t);
    }

    template<class T> Vector4<T> operator *(T t, const Vector4<T>& v) {
        return v * t;
    }

    template<class T> Vector4<T> operator /(const Vector4<T>& v, T t) {
        return Vector4<T>(v.X / t, v.Y / t, v.Z / t, v.W / t);
    }

    template<class T> Vector4<T> operator *(const Matrix4<T>& m, const Vector4<T>& v) {
        return Vector4<T>(m.a11 * v.X + m.a12 * v.Y + m.a13 * v.Z + m.a14 * v.W,
                          m.a21 * v.X + m.a22 * v.Y + m.a23 * v.Z + m.a24 * v.W,
                          m.a31 * v.X + m.a32 * v.Y + m.a33 * v.Z + m.a34 * v.W,
                          m.a41 * v.X + m.a42 * v.Y + m.a43 * v.Z + m.a44 * v.W);
    }

    template<class T> Vector4<T> operator *(const Vector4<T>& v, const Matrix4<T>& m) {
        return m * v;
    }

    template<class T> std::ostream& operator <<(std::ostream& stream, const Vector4<T>& v) {
        return stream << v.X << ", " << v.Y << ", " << v.Z << ", " << v.W;
    }

    //explicit template
    template class Vector4<float>;
    template Vector4<float> Vector4<float>::cast() const;
    template Vector4<double> Vector4<float>::cast() const;
    template Vector4<int> Vector4<float>::cast() const;
    template Vector4<float> operator *<float>(const Vector4<float>&, float);
    template Vector4<float> operator *<float>(float, const Vector4<float>&);
    template Vector4<float> operator /<float>(const Vector4<float>&, float);
    template Vector4<float> operator *<float>(const Matrix4<float>&, const Vector4<float>&);
    template Vector4<float> operator *<float>(const Vector4<float>&, const Matrix4<float>&);
    template std::ostream& operator <<<float>(std::ostream&, const Vector4<float>&);

    template class Vector4<double>;
    template Vector4<float> Vector4<double>::cast() const;
    template Vector4<double> Vector4<double>::cast() const;
    template Vector4<int> Vector4<double>::cast() const;
    template Vector4<double> operator *<double>(const Vector4<double>&, double);
    template Vector4<double> operator *<double>(double, const Vector4<double>&);
    template Vector4<double> operator /<double>(const Vector4<double>&, double);
    template Vector4<double> operator *<double>(const Matrix4<double>&, const Vector4<double>&);
    template Vector4<double> operator *<double>(const Vector4<double>&, const Matrix4<double>&);
    template std::ostream& operator <<<double>(std::ostream&, const Vector4<double>&);

    template class Vector4<int>;
    template Vector4<float> Vector4<int>::cast() const;
    template Vector4<double> Vector4<int>::cast() const;
    template Vector4<int> Vector4<int>::cast() const;
    template Vector4<int> operator *<int>(const Vector4<int>&, int);
    template Vector4<int> operator *<int>(int, const Vector4<int>&);
    template Vector4<int> operator /<int>(const Vector4<int>&, int);
    template Vector4<int> operator *<int>(const Matrix4<int>&, const Vector4<int>&);
    template Vector4<int> operator *<int>(const Vector4<int>&, const Matrix4<int>&);
    template std::ostream& operator <<<int>(std::ostream&, const Vector4<int>&);

}
