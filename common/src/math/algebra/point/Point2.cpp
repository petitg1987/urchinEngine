#include <math/algebra/point/Point2.h>

namespace urchin {

    template<class T> Point2<T>::Point2() :
            X(0), Y(0) {

    }

    template<class T> Point2<T>::Point2(T Xu, T Yu) :
            X(Xu), Y(Yu) {

    }

    template<class T> Point2<T>::Point2(const Vector2<T>& vector) :
            X(vector.X), Y(vector.Y) {

    }

    template<class T> void Point2<T>::setValues(T Xu, T Yu) {
        X = Xu;
        Y = Yu;
    }

    template<class T> void Point2<T>::setNull() {
        X = Y = 0;
    }

    template<class T> T Point2<T>::distance(const Point2<T>& p) const {
        return (T)std::sqrt(((X - p.X) * (X - p.X)) + ((Y - p.Y) * (Y - p.Y)));
    }

    template<class T> T Point2<T>::squareDistance(const Point2<T>& p) const {
        return (T)(((X - p.X) * (X - p.X)) + ((Y - p.Y) * (Y - p.Y)));
    }

    template<class T> Vector2<T> Point2<T>::vector(const Point2<T>& target) const {
        return Vector2<T>(target.X - X, target.Y - Y);
    }

    /**
     * @return Vector from origin (0.0, 0.0, 0.0) to this point
     */
    template<class T> Vector2<T> Point2<T>::toVector() const {
        return Vector2<T>(X, Y);
    }

    template<class T> Point2<T> Point2<T>::translate(const Vector2<T>& v) const {
        return Point2<T>(X + v.X, Y + v.Y);
    }

    template<class T> Point2<T> Point2<T>::operator +() const {
        return Point2<T>(X, Y);
    }

    template<class T> Point2<T> Point2<T>::operator -() const {
        return Point2<T>((T)-X, (T)-Y);
    }

    template<class T> Point2<T> Point2<T>::operator +(const Point2<T>& p) const {
        return Point2<T>(X + p.X,
                         Y + p.Y);
    }

    template<class T> Point2<T> Point2<T>::operator -(const Point2<T>& p) const {
        return Point2<T>(X - p.X,
                         Y - p.Y);
    }

    template<class T> const Point2<T>& Point2<T>::operator +=(const Point2<T>& p) {
        X += p.X;
        Y += p.Y;

        return *this;
    }

    template<class T> const Point2<T>& Point2<T>::operator -=(const Point2<T>& p) {
        X -= p.X;
        Y -= p.Y;

        return *this;
    }

    template<class T> const Point2<T>& Point2<T>::operator *=(T t) {
        X *= t;
        Y *= t;

        return *this;
    }

    template<class T> const Point2<T>& Point2<T>::operator /=(T t) {
        X /= t;
        Y /= t;

        return *this;
    }

    template<class T> const Point2<T>& Point2<T>::operator +=(T t) {
        X += t;
        Y += t;

        return *this;
    }

    template<class T> const Point2<T>& Point2<T>::operator -=(T t) {
        X -= t;
        Y -= t;

        return *this;
    }

    template<class T> bool Point2<T>::operator ==(const Point2<T>& p) const {
        return (X == p.X && Y == p.Y);
    }

    template<class T> std::partial_ordering Point2<T>::operator <=>(const Point2<T>& p) const {
        if (auto cmpX = X <=> p.X; cmpX != 0) {
            return cmpX;
        }
        return Y <=> p.Y;
    }

    template<class T> T& Point2<T>::operator [](std::size_t i) {
        return (&X)[i];
    }

    template<class T> const T& Point2<T>::operator [](std::size_t i) const {
        return (&X)[i];
    }

    template<class T> Point2<T>::operator T*() {
        return &X;
    }

    template<class T> Point2<T>::operator const T*() const {
        return &X;
    }

    template<class T> template<class NEW_TYPE> Point2<NEW_TYPE> Point2<T>::cast() const {
        return Point2<NEW_TYPE>((NEW_TYPE)X, (NEW_TYPE)Y);
    }

    template<class T> Point2<T> operator *(const Point2<T>& p, T t) {
        return Point2<T>(p.X * t, p.Y * t);
    }

    template<class T> Point2<T> operator *(T t, const Point2<T>& p) {
        return p * t;
    }

    template<class T> Point2<T> operator /(const Point2<T>& p, T t) {
        return Point2<T>(p.X / t, p.Y / t);
    }

    template<class T> Point2<T> operator +(const Point2<T>& p, T t) {
        return Point2<T>(p.X + t, p.Y + t);
    }

    template<class T> Point2<T> operator -(const Point2<T>& p, T t) {
        return Point2<T>(p.X - t, p.Y - t);
    }

    template<class T> Point2<T> operator *(const Matrix2<T>& m, const Point2<T>& p) {
        return Point2<T>(    m.a11 * p.X + m.a12 * p.Y,
                    m.a21 * p.X + m.a22 * p.Y);
    }

    template<class T> Point2<T> operator *(const Point2<T>& p, const Matrix2<T>& m) {
        return m * p;
    }

    template<class T> std::ostream& operator <<(std::ostream& stream, const Point2<T>& p) {
        return stream << p.X << ", " << p.Y;
    }

    //explicit template
    template class Point2<float>;
    template Point2<float> Point2<float>::cast() const;
    template Point2<double> Point2<float>::cast() const;
    template Point2<int> Point2<float>::cast() const;
    template Point2<long long> Point2<float>::cast() const;
    template Point2<float> operator *<float>(const Point2<float>&, float);
    template Point2<float> operator *<float>(float, const Point2<float>&);
    template Point2<float> operator /<float>(const Point2<float>&, float);
    template Point2<float> operator +<float>(const Point2<float>&, float);
    template Point2<float> operator -<float>(const Point2<float>&, float);
    template Point2<float> operator *<float>(const Matrix2<float>&, const Point2<float>&);
    template Point2<float> operator *<float>(const Point2<float>&, const Matrix2<float>&);
    template std::ostream& operator <<<float>(std::ostream&, const Point2<float>&);

    template class Point2<double>;
    template Point2<float> Point2<double>::cast() const;
    template Point2<double> Point2<double>::cast() const;
    template Point2<int> Point2<double>::cast() const;
    template Point2<long long> Point2<double>::cast() const;
    template Point2<double> operator *<double>(const Point2<double>&, double);
    template Point2<double> operator *<double>(double, const Point2<double>&);
    template Point2<double> operator /<double>(const Point2<double>&, double);
    template Point2<double> operator +<double>(const Point2<double>&, double);
    template Point2<double> operator -<double>(const Point2<double>&, double);
    template Point2<double> operator *<double>(const Matrix2<double>&, const Point2<double>&);
    template Point2<double> operator *<double>(const Point2<double>&, const Matrix2<double>&);
    template std::ostream& operator <<<double>(std::ostream&, const Point2<double>&);

    template class Point2<int>;
    template Point2<float> Point2<int>::cast() const;
    template Point2<double> Point2<int>::cast() const;
    template Point2<int> Point2<int>::cast() const;
    template Point2<long long> Point2<int>::cast() const;
    template Point2<int> operator *<int>(const Point2<int>&, int);
    template Point2<int> operator *<int>(int, const Point2<int>&);
    template Point2<int> operator /<int>(const Point2<int>&, int);
    template Point2<int> operator +<int>(const Point2<int>&, int);
    template Point2<int> operator -<int>(const Point2<int>&, int);
    template Point2<int> operator *<int>(const Matrix2<int>&, const Point2<int>&);
    template Point2<int> operator *<int>(const Point2<int>&, const Matrix2<int>&);
    template std::ostream& operator <<<int>(std::ostream&, const Point2<int>&);

    template class Point2<unsigned int>;
    template Point2<float> Point2<unsigned int>::cast() const;
    template Point2<double> Point2<unsigned int>::cast() const;
    template Point2<unsigned int> Point2<unsigned int>::cast() const;
    template Point2<long long> Point2<unsigned int>::cast() const;
    template Point2<unsigned int> operator *<unsigned int>(const Point2<unsigned int>&, unsigned int);
    template Point2<unsigned int> operator *<unsigned int>(unsigned int, const Point2<unsigned int>&);
    template Point2<unsigned int> operator /<unsigned int>(const Point2<unsigned int>&, unsigned int);
    template Point2<unsigned int> operator +<unsigned int>(const Point2<unsigned int>&, unsigned int);
    template Point2<unsigned int> operator -<unsigned int>(const Point2<unsigned int>&, unsigned int);
    template Point2<unsigned int> operator *<unsigned int>(const Matrix2<unsigned int>&, const Point2<unsigned int>&);
    template Point2<unsigned int> operator *<unsigned int>(const Point2<unsigned int>&, const Matrix2<unsigned int>&);
    template std::ostream& operator <<<unsigned int>(std::ostream&, const Point2<unsigned int>&);

    template class Point2<long long>;
    template Point2<float> Point2<long long>::cast() const;
    template Point2<double> Point2<long long>::cast() const;
    template Point2<int> Point2<long long>::cast() const;
    template Point2<long long > Point2<long long>::cast() const;
    template Point2<long long> operator *<long long>(const Point2<long long>&, long long);
    template Point2<long long> operator *<long long>(long long, const Point2<long long>&);
    template Point2<long long> operator /<long long>(const Point2<long long>&, long long);
    template Point2<long long> operator +<long long>(const Point2<long long>&, long long);
    template Point2<long long> operator -<long long>(const Point2<long long>&, long long);
    template Point2<long long> operator *<long long>(const Matrix2<long long>&, const Point2<long long>&);
    template Point2<long long> operator *<long long>(const Point2<long long>&, const Matrix2<long long>&);
    template std::ostream& operator <<<long long>(std::ostream&, const Point2<long long>&);

}
