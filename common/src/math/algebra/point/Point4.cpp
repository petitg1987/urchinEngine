#include <math/algebra/point/Point4.h>

namespace urchin {

    template<class T> Point4<T>::Point4() :
            X(0), Y(0), Z(0), W(1) {

    }

    template<class T> Point4<T>::Point4(T Xu, T Yu, T Zu, T Wu) :
            X(Xu), Y(Yu), Z(Zu), W(Wu) {

    }

    template<class T> Point4<T>::Point4(const Point2<T>& p, T Zu, T Wu) :
            X(p.X), Y(p.Y), Z(Zu), W(Wu) {

    }

    template<class T> Point4<T>::Point4(const Point3<T>& p, T Wu) :
            X(p.X), Y(p.Y), Z(p.Z), W(Wu) {

    }

    template<class T> Point4<T>::Point4(const Vector3<T>& vector, T Wu) :
            X(vector.X), Y(vector.Y), Z(vector.Z), W(Wu) {

    }

    template<class T> Point4<T>::Point4(const Vector4<T>& vector) :
            X(vector.X), Y(vector.Y), Z(vector.Z), W(vector.W) {

    }

    template<class T> void Point4<T>::setValues(T Xu, T Yu, T Zu, T Wu) {
        X = Xu;
        Y = Yu;
        Z = Zu;
        W = Wu;
    }

    template<class T> void Point4<T>::setNull() {
        X = Y = Z = W = 0;
    }

    template<class T> T Point4<T>::distance(const Point4<T>& p) const {
        return (T)std::sqrt(((X - p.X) * (X - p.X)) + ((Y - p.Y) * (Y - p.Y)) + ((Z - p.Z) * (Z - p.Z)) + ((W - p.W) * (W - p.W)));
    }

    template<class T> T Point4<T>::squareDistance(const Point4<T>& p) const {
        return (T)(((X - p.X) * (X - p.X)) + ((Y - p.Y) * (Y - p.Y)) + ((Z - p.Z) * (Z - p.Z)) + ((W - p.W) * (W - p.W)));
    }

    template<class T> Vector4<T> Point4<T>::vector(const Point4<T>& target) const {
        return Vector4<T>(target.X - X, target.Y - Y, target.Z - Z, target.W - W);
    }

    /**
     * @return Vector from origin (0.0, 0.0, 0.0) to this point
     */
    template<class T> Vector4<T> Point4<T>::toVector() const {
        return Vector4<T>(X, Y, Z, W);
    }

    template<class T> Point4<T> Point4<T>::translate(const Vector4<T>& v) const {
        return Point4<T>(X + v.X, Y + v.Y, Z + v.Z, W + v.W);
    }

    template<class T> Point4<T> Point4<T>::divideByW() const {
        if (W != 0.0) [[likely]] {
            return Point4<T>(X / W, Y / W, Z / W, 1);
        }

        return *this;
    }

    template<class T> Point3<T> Point4<T>::toPoint3() const {
        if (W != 0.0) [[likely]] {
            return Point3<T>(X / W, Y / W, Z / W);
        }

        return Point3<T>(X, Y, Z);
    }

    template<class T> Point4<T> Point4<T>::operator +() const {
        return Point4<T>(X, Y, Z, W);
    }

    template<class T> Point4<T> Point4<T>::operator -() const {
        return Point4<T>(-X, -Y, -Z, -W);
    }

    template<class T> Point4<T> Point4<T>::operator +(const Point4<T>& p) const {
        return Point4<T>(X + p.X,
                         Y + p.Y,
                         Z + p.Z,
                         W + p.W);
    }

    template<class T> Point4<T> Point4<T>::operator -(const Point4<T>& p) const {
        return Point4<T>(X - p.X,
                         Y - p.Y,
                         Z - p.Z,
                         W - p.W);
    }

    template<class T> const Point4<T>& Point4<T>::operator +=(const Point4<T>& p) {
        X += p.X;
        Y += p.Y;
        Z += p.Z;
        W += p.W;

        return *this;
    }

    template<class T> const Point4<T>& Point4<T>::operator -=(const Point4<T>& p) {
        X -= p.X;
        Y -= p.Y;
        Z -= p.Z;
        W -= p.W;

        return *this;
    }

    template<class T> const Point4<T>& Point4<T>::operator *=(T t) {
        X *= t;
        Y *= t;
        Z *= t;
        W *= t;

        return *this;
    }

    template<class T> const Point4<T>& Point4<T>::operator /=(T t) {
        X /= t;
        Y /= t;
        Z /= t;
        W /= t;

        return *this;
    }

    template<class T> const Point4<T>& Point4<T>::operator +=(T t) {
        X += t;
        Y += t;
        Z += t;
        W += t;

        return *this;
    }

    template<class T> const Point4<T>& Point4<T>::operator -=(T t) {
        X -= t;
        Y -= t;
        Z -= t;
        W -= t;

        return *this;
    }

    template<class T> bool Point4<T>::operator ==(const Point4<T>& p) const {
        return (X == p.X && Y == p.Y && Z == p.Z && W == p.W);
    }

    template<class T> std::partial_ordering Point4<T>::operator <=>(const Point4<T>& p) const {
        if (auto cmpX = X <=> p.X; cmpX != 0) {
            return cmpX;
        } else if (auto cmpY = Y <=> p.Y; cmpY != 0) {
            return cmpY;
        } else if (auto cmpZ = Z <=> p.Z; cmpZ != 0) {
            return cmpZ;
        }
        return W <=> p.W;
    }

    template<class T> T& Point4<T>::operator [](std::size_t i) {
        return (&X)[i];
    }

    template<class T> const T& Point4<T>::operator [](std::size_t i) const {
        return (&X)[i];
    }

    template<class T> Point4<T>::operator T*() {
        return &X;
    }

    template<class T> Point4<T>::operator const T*() const {
        return &X;
    }

    template<class T> template<class NEW_TYPE> Point4<NEW_TYPE> Point4<T>::cast() const {
        return Point4<NEW_TYPE>((NEW_TYPE)X, (NEW_TYPE)Y, (NEW_TYPE)Z, (NEW_TYPE)W);
    }

    template<class T> Point4<T> operator *(const Point4<T>& p, T t) {
        return Point4<T>(p.X * t, p.Y * t, p.Z * t, p.W * t);
    }

    template<class T> Point4<T> operator *(T t, const Point4<T>& p) {
        return p * t;
    }

    template<class T> Point4<T> operator /(const Point4<T>& p, T t) {
        return Point4<T>(p.X / t, p.Y / t, p.Z / t, p.W / t);
    }

    template<class T> Point4<T> operator +(const Point4<T>& p, T t) {
        return Point4<T>(p.X + t, p.Y + t, p.Z + t, p.W + t);
    }

    template<class T> Point4<T> operator -(const Point4<T>& p, T t) {
        return Point4<T>(p.X - t, p.Y - t, p.Z - t, p.W - t);
    }

    template<class T> Point4<T> operator *(const Matrix4<T>& m, const Point4<T>& p) {
        return Point4<T>(    m.a11 * p.X + m.a12 * p.Y + m.a13 * p.Z + m.a14 * p.W,
                    m.a21 * p.X + m.a22 * p.Y + m.a23 * p.Z + m.a24 * p.W,
                    m.a31 * p.X + m.a32 * p.Y + m.a33 * p.Z + m.a34 * p.W,
                    m.a41 * p.X + m.a42 * p.Y + m.a43 * p.Z + m.a44 * p.W);
    }

    template<class T> Point4<T> operator *(const Point4<T>& p, const Matrix4<T>& m) {
        return m * p;
    }

    template<class T> std::ostream& operator <<(std::ostream& stream, const Point4<T>& p) {
        return stream << p.X << ", " << p.Y << ", " << p.Z << ", " << p.W;
    }

    //explicit template
    template class Point4<float>;
    template Point4<float> Point4<float>::cast() const;
    template Point4<double> Point4<float>::cast() const;
    template Point4<int> Point4<float>::cast() const;
    template Point4<float> operator *<float>(const Point4<float>&, float);
    template Point4<float> operator *<float>(float, const Point4<float>&);
    template Point4<float> operator /<float>(const Point4<float>&, float);
    template Point4<float> operator +<float>(const Point4<float>&, float);
    template Point4<float> operator -<float>(const Point4<float>&, float);
    template Point4<float> operator *<float>(const Matrix4<float>&, const Point4<float>&);
    template Point4<float> operator *<float>(const Point4<float>&, const Matrix4<float>&);
    template std::ostream& operator <<<float>(std::ostream&, const Point4<float>&);

    template class Point4<double>;
    template Point4<float> Point4<double>::cast() const;
    template Point4<double> Point4<double>::cast() const;
    template Point4<int> Point4<double>::cast() const;
    template Point4<double> operator *<double>(const Point4<double>&, double);
    template Point4<double> operator *<double>(double, const Point4<double>&);
    template Point4<double> operator /<double>(const Point4<double>&, double);
    template Point4<double> operator +<double>(const Point4<double>&, double);
    template Point4<double> operator -<double>(const Point4<double>&, double);
    template Point4<double> operator *<double>(const Matrix4<double>&, const Point4<double>&);
    template Point4<double> operator *<double>(const Point4<double>&, const Matrix4<double>&);
    template std::ostream& operator <<<double>(std::ostream&, const Point4<double>&);

    template class Point4<int>;
    template Point4<float> Point4<int>::cast() const;
    template Point4<double> Point4<int>::cast() const;
    template Point4<int> Point4<int>::cast() const;
    template Point4<int> operator *<int>(const Point4<int>&, int);
    template Point4<int> operator *<int>(int, const Point4<int>&);
    template Point4<int> operator /<int>(const Point4<int>&, int);
    template Point4<int> operator +<int>(const Point4<int>&, int);
    template Point4<int> operator -<int>(const Point4<int>&, int);
    template Point4<int> operator *<int>(const Matrix4<int>&, const Point4<int>&);
    template Point4<int> operator *<int>(const Point4<int>&, const Matrix4<int>&);
    template std::ostream& operator <<<int>(std::ostream&, const Point4<int>&);

}
