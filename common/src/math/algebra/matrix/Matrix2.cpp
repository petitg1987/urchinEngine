#include "math/algebra/matrix/Matrix2.h"

namespace urchin {

    /**
     * Construct an identify matrix
     */
    template<class T> Matrix2<T>::Matrix2() :
            a11(1.0), a21(0.0),
            a12(0.0), a22(1.0) {

    }

    template<class T> Matrix2<T>::Matrix2(T m11, T m12,
        T m21, T m22) :
            a11(m11), a21(m21),
            a12(m12), a22(m22) {

    }

    template<class T> void Matrix2<T>::setValues(T m11, T m12,
            T m21, T m22) {
        a11=m11; a12=m12;
        a21=m21; a22=m22;
    }

    template<class T> void Matrix2<T>::setIdentity() {
        a11 = 1.0; a12 = 0.0;
        a21 = 0.0; a22 = 1.0;
    }

    template<class T> T Matrix2<T>::determinant() const {
        return a11 * a22 - a12 * a21;
    }

    /**
     * Return transpose matrix. In case of rotation matrix: transpose method return same result as inverse method but with better performance.
     */
    template<class T> Matrix2<T> Matrix2<T>::transpose() const {
        return Matrix2<T>(
                a11, a21,
                a12, a22);
    }

    /**
     * Return inverse matrix. In case of rotation matrix: inverse method return same result as transpose method but with worst performance.
     */
    template<class T> Matrix2<T> Matrix2<T>::inverse() const {
        Matrix2<T> ret;
        T det = determinant();

        if (std::fabs(det) > 0.0) {
            ret.a11 =  a22 / det;
            ret.a12 = -a12 / det;

            ret.a21 = -a21 / det;
            ret.a22 =  a11 / det;
        }

        return ret;
    }

    template<class T> Matrix2<T> Matrix2<T>::scaled(T scaleFactorColumn0, T scaleFactorColumn1) const {
        return Matrix2<T>(a11*scaleFactorColumn0, a12*scaleFactorColumn1,
                a21*scaleFactorColumn0, a22*scaleFactorColumn1);
    }

    template<class T> void Matrix2<T>::buildScale(T x, T y) {
        a11 = x; a12 = 0.0;
        a21 = 0.0; a22 = y;
    }

    template<class T> Matrix2<T> Matrix2<T>::operator +() const {
        return *this;
    }

    template<class T> Matrix2<T> Matrix2<T>::operator -() const {
        return Matrix2<T>(    -a11, -a12,
                -a21, -a22);
    }

    template<class T> Matrix2<T> Matrix2<T>::operator +(const Matrix2<T>& m) const {
        return Matrix2<T>(    a11 + m.a11, a12 + m.a12,
                a21 + m.a21, a22 + m.a22);
    }

    template<class T> Matrix2<T> Matrix2<T>::operator -(const Matrix2& m) const {
        return Matrix2(    a11 - m.a11, a12 - m.a12,
                a21 - m.a21, a22 - m.a22);
    }

    template<class T> const Matrix2<T>& Matrix2<T>::operator +=(const Matrix2<T>& m) {
        a11 += m.a11; a12 += m.a12;
        a21 += m.a21; a22 += m.a22;

        return *this;
    }

    template<class T> const Matrix2<T>& Matrix2<T>::operator -=(const Matrix2<T>& m) {
        a11 -= m.a11; a12 -= m.a12;
        a21 -= m.a21; a22 -= m.a22;

        return *this;
    }

    template<class T> Matrix2<T> Matrix2<T>::operator *(const Matrix2<T>& m) const {
        return Matrix2<T>(
                a11 * m.a11 + a12 * m.a21,
                a11 * m.a12 + a12 * m.a22,

                a21 * m.a11 + a22 * m.a21,
                a21 * m.a12 + a22 * m.a22);
    }

    template<class T> const Matrix2<T>& Matrix2<T>::operator *=(const Matrix2<T>& m) {
        *this = *this * m;

        return *this;
    }

    template<class T> const Matrix2<T>& Matrix2<T>::operator *=(T t) {
        a11 *= t; a12 *= t;
        a21 *= t; a22 *= t;

        return *this;
    }

    template<class T> const Matrix2<T>& Matrix2<T>::operator /=(T t) {
        a11 /= t; a12 /= t;
        a21 /= t; a22 /= t;

        return *this;
    }

    template<class T> T& Matrix2<T>::operator ()(size_t line, size_t column) {
        return (&a11)[column * 2 + line];
    }

    template<class T> const T& Matrix2<T>::operator ()(size_t line, size_t column) const {
        return (&a11)[column * 2 + line];
    }

    template<class T> T& Matrix2<T>::operator ()(std::size_t index) {
        return (&a11)[index];
    }

    template<class T> const T& Matrix2<T>::operator ()(std::size_t index) const {
        return (&a11)[index];
    }

    template<class T> Matrix2<T>::operator T*() {
        return &a11;
    }

    template<class T> Matrix2<T>::operator const T*() const {
        return &a11;
    }

    template<class T> Matrix2<T> operator *(const Matrix2<T>& m, T t) {
        return Matrix2<T>(    m.a11 * t, m.a12 * t,
                m.a21 * t, m.a22 * t);
    }

    template<class T> Matrix2<T> operator *(T t, const Matrix2<T>& m) {
        return m * t;
    }

    template<class T> Matrix2<T> operator /(const Matrix2<T>& m, T t) {
        return Matrix2<T>(    m.a11 / t, m.a12 / t,
                m.a21 / t, m.a22 / t);
    }

    template<class T> std::ostream& operator <<(std::ostream& stream, const Matrix2<T>& m) {
        stream.setf(std::ios::left);
        stream << std::setw(10) << m.a11 << " " << std::setw(10) << m.a12 << std::endl;
        stream << std::setw(10) << m.a21 << " " << std::setw(10) << m.a22;

        return stream;
    }

    //explicit template
    template class Matrix2<float>;
    template Matrix2<float> operator *<float>(const Matrix2<float>&, float);
    template Matrix2<float> operator *<float>(float, const Matrix2<float>&);
    template Matrix2<float> operator /<float>(const Matrix2<float>&, float);
    template std::ostream& operator <<<float>(std::ostream&, const Matrix2<float>&);

    template class Matrix2<double>;
    template Matrix2<double> operator *<double>(const Matrix2<double>&, double);
    template Matrix2<double> operator *<double>(double, const Matrix2<double>&);
    template Matrix2<double> operator /<double>(const Matrix2<double>&, double);
    template std::ostream& operator <<<double>(std::ostream&, const Matrix2<double>&);

}
