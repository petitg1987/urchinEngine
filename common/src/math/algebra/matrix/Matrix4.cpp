#include <math/algebra/matrix/Matrix4.h>

namespace urchin {

    /**
     * Construct an identify matrix
     */
    template<class T> Matrix4<T>::Matrix4() :
            a11(1.0), a21(0.0), a31(0.0), a41(0.0),
            a12(0.0), a22(1.0), a32(0.0), a42(0.0),
            a13(0.0), a23(0.0), a33(1.0), a43(0.0),
            a14(0.0), a24(0.0), a34(0.0), a44(1.0) {

    }

    template<class T> Matrix4<T>::Matrix4(T m11, T m12, T m13, T m14,
        T m21, T m22, T m23, T m24,
        T m31, T m32, T m33, T m34,
        T m41, T m42, T m43, T m44) :
            a11(m11), a21(m21), a31(m31), a41(m41),
            a12(m12), a22(m22), a32(m32), a42(m42),
            a13(m13), a23(m23), a33(m33), a43(m43),
            a14(m14), a24(m24), a34(m34), a44(m44) {

    }

    template<class T> void Matrix4<T>::setValues(T m11, T m12, T m13, T m14,
        T m21, T m22, T m23, T m24,
        T m31, T m32, T m33, T m34,
        T m41, T m42, T m43, T m44) {
        a11 = m11; a12 = m12; a13 = m13; a14 = m14;
        a21 = m21; a22 = m22; a23 = m23; a24 = m24;
        a31 = m31; a32 = m32; a33 = m33; a34 = m34;
        a41 = m41; a42 = m42; a43 = m43; a44 = m44;
    }

    template<class T> void Matrix4<T>::setIdentity() {
        a11 = 1.0; a12 = 0.0; a13 = 0.0; a14 = 0.0;
        a21 = 0.0; a22 = 1.0; a23 = 0.0; a24 = 0.0;
        a31 = 0.0; a32 = 0.0; a33 = 1.0; a34 = 0.0;
        a41 = 0.0; a42 = 0.0; a43 = 0.0; a44 = 1.0;
    }

    template<class T> T Matrix4<T>::determinant() const {
        const T A = a22 * (a33 * a44 - a43 * a34) - a32 * (a23 * a44 - a43 * a24) + a42 * (a23 * a34 - a33 * a24);
        const T B = a12 * (a33 * a44 - a43 * a34) - a32 * (a13 * a44 - a43 * a14) + a42 * (a13 * a34 - a33 * a14);
        const T C = a12 * (a23 * a44 - a43 * a24) - a22 * (a13 * a44 - a43 * a14) + a42 * (a13 * a24 - a23 * a14);
        const T D = a12 * (a23 * a34 - a33 * a24) - a22 * (a13 * a34 - a33 * a14) + a32 * (a13 * a24 - a23 * a14);

        return a11 * A - a21 * B + a31 * C - a41 * D;
    }

    /**
     * Return transpose matrix. In case of rotation matrix: transpose method return same result as inverse method but with better performance.
     */
    template<class T> Matrix4<T> Matrix4<T>::transpose() const {
        return Matrix4<T>(
                a11, a21, a31, a41,
                a12, a22, a32, a42,
                a13, a23, a33, a43,
                a14, a24, a34, a44);
    }

    /**
     * Return inverse matrix. In case of rotation matrix: inverse method return same result as transpose method but with worst performance.
     */
    template<class T> Matrix4<T> Matrix4<T>::inverse() const {
        Matrix4 ret;
        T det = determinant();

        if (std::fabs(det) > 0.0) {
            ret.a11 =  (a22 * (a33 * a44 - a34 * a43) - a32 * (a23 * a44 - a43 * a24) + a42 * (a23 * a34 - a33 *  a24)) / det;
            ret.a12 = -(a12 * (a33 * a44 - a43 * a34) - a32 * (a13 * a44 - a43 * a14) + a42 * (a13 * a34 - a33 *  a14)) / det;
            ret.a13 =  (a12 * (a23 * a44 - a43 * a24) - a22 * (a13 * a44 - a43 * a14) + a42 * (a13 * a24 - a23 *  a14)) / det;
            ret.a14 = -(a12 * (a23 * a34 - a33 * a24) - a22 * (a13 * a34 - a33 * a14) + a32 * (a13 * a24 - a23 *  a14)) / det;

            ret.a21 = -(a21 * (a33 * a44 - a34 * a43) - a23 * (a31 * a44 - a34 * a41) + a24 * (a31 * a43 - a33 *  a41)) / det;
            ret.a22 =  (a11 * (a33 * a44 - a34 * a43) - a13 * (a31 * a44 - a34 * a41) + a14 * (a31 * a43 - a33 *  a41)) / det;
            ret.a23 = -(a11 * (a23 * a44 - a24 * a43) - a13 * (a21 * a44 - a24 * a41) + a14 * (a21 * a43 - a23 *  a41)) / det;
            ret.a24 =  (a11 * (a23 * a34 - a24 * a33) - a13 * (a21 * a34 - a24 * a31) + a14 * (a21 * a33 - a23 *  a31)) / det;

            ret.a31 =  (a21 * (a32 * a44 - a34 * a42) - a22 * (a31 * a44 - a34 * a41) + a24 * (a31 * a42 - a32 *  a41)) / det;
            ret.a32 = -(a11 * (a32 * a44 - a34 * a42) - a12 * (a31 * a44 - a34 * a41) + a14 * (a31 * a42 - a32 *  a41)) / det;
            ret.a33 =  (a11 * (a22 * a44 - a24 * a42) - a12 * (a21 * a44 - a24 * a41) + a14 * (a21 * a42 - a22 *  a41)) / det;
            ret.a34 = -(a11 * (a22 * a34 - a24 * a32) - a12 * (a21 * a34 - a24 * a31) + a14 * (a21 * a32 - a22 *  a31)) / det;

            ret.a41 = -(a21 * (a32 * a43 - a33 * a42) - a22 * (a31 * a43 - a33 * a41) + a23 * (a31 * a42 - a32 *  a41)) / det;
            ret.a42 =  (a11 * (a32 * a43 - a33 * a42) - a12 * (a31 * a43 - a33 * a41) + a13 * (a31 * a42 - a32 *  a41)) / det;
            ret.a43 = -(a11 * (a22 * a43 - a23 * a42) - a12 * (a21 * a43 - a23 * a41) + a13 * (a21 * a42 - a22 *  a41)) / det;
            ret.a44 =  (a11 * (a22 * a33 - a23 * a32) - a12 * (a21 * a33 - a23 * a31) + a13 * (a21 * a32 - a22 *  a31)) / det;
        }

        return ret;
    }

    template<class T> Matrix3<T> Matrix4<T>::toMatrix3() const {
        return Matrix3<T>(a11, a12, a13,
                          a21, a22, a23,
                          a31, a32, a33);
    }

    template<class T> Matrix4<T> Matrix4<T>::scaled(T scaleFactorColumn0, T scaleFactorColumn1, T scaleFactorColumn2, T scaleFactorColumn3) const {
        return Matrix4<T>(a11 * scaleFactorColumn0, a12 * scaleFactorColumn1, a13 * scaleFactorColumn2, a14 * scaleFactorColumn3,
                          a21 * scaleFactorColumn0, a22 * scaleFactorColumn1, a23 * scaleFactorColumn2, a24 * scaleFactorColumn3,
                          a31 * scaleFactorColumn0, a32 * scaleFactorColumn1, a33 * scaleFactorColumn2, a34 * scaleFactorColumn3,
                          a41 * scaleFactorColumn0, a42 * scaleFactorColumn1, a43 * scaleFactorColumn2, a44 * scaleFactorColumn3);
    }

    template<class T> void Matrix4<T>::buildTranslation(T x, T y, T z) {
        a11 = 1.0; a12 = 0.0; a13 = 0.0; a14 = x;
        a21 = 0.0; a22 = 1.0; a23 = 0.0; a24 = y;
        a31 = 0.0; a32 = 0.0; a33 = 1.0; a34 = z;
        a41 = 0.0; a42 = 0.0; a43 = 0.0; a44 = 1.0;
    }

    template<class T> void Matrix4<T>::buildScale(T x, T y, T z) {
        a11 = x;    a12 = 0.0; a13 = 0.0; a14 = 0.0;
        a21 = 0.0; a22 = y;    a23 = 0.0; a24 = 0.0;
        a31 = 0.0; a32 = 0.0; a33 = z;    a34 = 0.0;
        a41 = 0.0; a42 = 0.0; a43 = 0.0; a44 = 1.0;
    }

    template<class T> void Matrix4<T>::buildRotationX(T angle) {
        const T cos = std::cos(angle);
        const T sin = std::sin(angle);

        a11 = 1.0; a12 = 0.0; a13 = 0.0; a14 = 0.0;
        a21 = 0.0; a22 = cos;  a23 = -sin; a24 = 0.0;
        a31 = 0.0; a32 = sin;  a33 = cos;  a34 = 0.0;
        a41 = 0.0; a42 = 0.0; a43 = 0.0; a44 = 1.0;
    }

    template<class T> void Matrix4<T>::buildRotationY(T angle) {
        const T cos = std::cos(angle);
        const T sin = std::sin(angle);

        a11 = cos;  a12 = 0.0; a13 = sin; a14 = 0.0;
        a21 = 0.0; a22 = 1.0; a23 = 0.0; a24 = 0.0;
        a31 = -sin;  a32 = 0.0; a33 = cos;  a34 = 0.0;
        a41 = 0.0; a42 = 0.0; a43 = 0.0; a44 = 1.0;
    }

    template<class T> void Matrix4<T>::buildRotationZ(T angle) {
        const T cos = std::cos(angle);
        const T sin = std::sin(angle);

        a11 = cos;  a12 = -sin;  a13 = 0.0; a14 = 0.0;
        a21 = sin;  a22 = cos;  a23 = 0.0; a24 = 0.0;
        a31 = 0.0; a32 = 0.0; a33 = 1.0; a34 = 0.0;
        a41 = 0.0; a42 = 0.0; a43 = 0.0; a44 = 1.0;
    }

    template<class T> Matrix4<T> Matrix4<T>::operator +() const {
        return *this;
    }

    template<class T> Matrix4<T> Matrix4<T>::operator -() const {
        return Matrix4<T>(    -a11, -a12, -a13, -a14,
                -a21, -a22, -a23, -a24,
                -a31, -a32, -a33, -a34,
                -a41, -a42, -a43, -a44);
    }

    template<class T> Matrix4<T> Matrix4<T>::operator +(const Matrix4& m) const {
        return Matrix4<T>(    a11 + m.a11, a12 + m.a12, a13 + m.a13, a14 + m.a14,
                a21 + m.a21, a22 + m.a22, a23 + m.a23, a24 + m.a24,
                a31 + m.a31, a32 + m.a32, a33 + m.a33, a34 + m.a34,
                a41 + m.a41, a42 + m.a42, a43 + m.a43, a44 + m.a44);
    }

    template<class T> Matrix4<T> Matrix4<T>::operator -(const Matrix4<T>& m) const {
        return Matrix4(    a11 - m.a11, a12 - m.a12, a13 - m.a13, a14 - m.a14,
                a21 - m.a21, a22 - m.a22, a23 - m.a23, a24 - m.a24,
                a31 - m.a31, a32 - m.a32, a33 - m.a33, a34 - m.a34,
                a41 - m.a41, a42 - m.a42, a43 - m.a43, a44 - m.a44);
    }

    template<class T> const Matrix4<T>& Matrix4<T>::operator +=(const Matrix4<T>& m) {
        a11 += m.a11; a12 += m.a12; a13 += m.a13; a14 += m.a14;
        a21 += m.a21; a22 += m.a22; a23 += m.a23; a24 += m.a24;
        a31 += m.a31; a32 += m.a32; a33 += m.a33; a34 += m.a34;
        a41 += m.a41; a42 += m.a42; a43 += m.a43; a44 += m.a44;

        return *this;
    }

    template<class T> const Matrix4<T>& Matrix4<T>::operator -=(const Matrix4<T>& m) {
        a11 -= m.a11; a12 -= m.a12; a13 -= m.a13; a14 -= m.a14;
        a21 -= m.a21; a22 -= m.a22; a23 -= m.a23; a24 -= m.a24;
        a31 -= m.a31; a32 -= m.a32; a33 -= m.a33; a34 -= m.a34;
        a41 -= m.a41; a42 -= m.a42; a43 -= m.a43; a44 -= m.a44;

        return *this;
    }

    template<class T> Matrix4<T> Matrix4<T>::operator *(const Matrix4<T>& m) const {
        return Matrix4<T>(
                a11 * m.a11 + a12 * m.a21 + a13 * m.a31 + a14 * m.a41,
                a11 * m.a12 + a12 * m.a22 + a13 * m.a32 + a14 * m.a42,
                a11 * m.a13 + a12 * m.a23 + a13 * m.a33 + a14 * m.a43,
                a11 * m.a14 + a12 * m.a24 + a13 * m.a34 + a14 * m.a44,

                a21 * m.a11 + a22 * m.a21 + a23 * m.a31 + a24 * m.a41,
                a21 * m.a12 + a22 * m.a22 + a23 * m.a32 + a24 * m.a42,
                a21 * m.a13 + a22 * m.a23 + a23 * m.a33 + a24 * m.a43,
                a21 * m.a14 + a22 * m.a24 + a23 * m.a34 + a24 * m.a44,

                a31 * m.a11 + a32 * m.a21 + a33 * m.a31 + a34 * m.a41,
                a31 * m.a12 + a32 * m.a22 + a33 * m.a32 + a34 * m.a42,
                a31 * m.a13 + a32 * m.a23 + a33 * m.a33 + a34 * m.a43,
                a31 * m.a14 + a32 * m.a24 + a33 * m.a34 + a34 * m.a44,

                a41 * m.a11 + a42 * m.a21 + a43 * m.a31 + a44 * m.a41,
                a41 * m.a12 + a42 * m.a22 + a43 * m.a32 + a44 * m.a42,
                a41 * m.a13 + a42 * m.a23 + a43 * m.a33 + a44 * m.a43,
                a41 * m.a14 + a42 * m.a24 + a43 * m.a34 + a44 * m.a44);
    }

    template<class T> const Matrix4<T>& Matrix4<T>::operator *=(const Matrix4& m) {
        *this = *this * m;

        return *this;
    }

    template<class T> const Matrix4<T>& Matrix4<T>::operator *=(T t) {
        a11 *= t; a12 *= t; a13 *= t; a14 *= t;
        a21 *= t; a22 *= t; a23 *= t; a24 *= t;
        a31 *= t; a32 *= t; a33 *= t; a34 *= t;
        a41 *= t; a42 *= t; a43 *= t; a44 *= t;

        return *this;
    }

    template<class T> const Matrix4<T>& Matrix4<T>::operator /=(T t) {
        a11 /= t; a12 /= t; a13 /= t; a14 /= t;
        a21 /= t; a22 /= t; a23 /= t; a24 /= t;
        a31 /= t; a32 /= t; a33 /= t; a34 /= t;
        a41 /= t; a42 /= t; a43 /= t; a44 /= t;

        return *this;
    }

    template<class T> T& Matrix4<T>::operator ()(size_t line, size_t column) {
        return (&a11)[column * 4 + line];
    }

    template<class T> const T& Matrix4<T>::operator ()(size_t line, size_t column) const {
        return (&a11)[column * 4 + line];
    }

    template<class T> T& Matrix4<T>::operator ()(std::size_t index) {
        return (&a11)[index];
    }

    template<class T> const T& Matrix4<T>::operator ()(std::size_t index) const {
        return (&a11)[index];
    }

    template<class T> Matrix4<T>::operator T*() {
        return &a11;
    }

    template<class T> Matrix4<T>::operator const T*() const {
        return &a11;
    }

    template<class T> Matrix4<T> operator *(const Matrix4<T>& m, T t) {
        return Matrix4<T>(    m.a11 * t, m.a12 * t, m.a13 * t, m.a14 * t,
                m.a21 * t, m.a22 * t, m.a23 * t, m.a24 * t,
                m.a31 * t, m.a32 * t, m.a33 * t, m.a34 * t,
                m.a41 * t, m.a42 * t, m.a43 * t, m.a44 * t);
    }

    template<class T> Matrix4<T> operator *(T t, const Matrix4<T>& m) {
        return m * t;
    }

    template<class T> Matrix4<T> operator /(const Matrix4<T>& m, T t) {
        return Matrix4<T>(    m.a11 / t, m.a12 / t, m.a13 / t, m.a14 / t,
                m.a21 / t, m.a22 / t, m.a23 / t, m.a24 / t,
                m.a31 / t, m.a32 / t, m.a33 / t, m.a34 / t,
                m.a41 / t, m.a42 / t, m.a43 / t, m.a44 / t);
    }

    template<class T> std::ostream& operator <<(std::ostream& stream, const Matrix4<T>& m) {
        stream.setf(std::ios::left);
        stream << std::setw(10) << m.a11 << " " << std::setw(10) << m.a12 << " " << std::setw(10) << m.a13 << " " << std::setw(10) << m.a14 << std::endl;
        stream << std::setw(10) << m.a21 << " " << std::setw(10) << m.a22 << " " << std::setw(10) << m.a23 << " " << std::setw(10) << m.a24 << std::endl;
        stream << std::setw(10) << m.a31 << " " << std::setw(10) << m.a32 << " " << std::setw(10) << m.a33 << " " << std::setw(10) << m.a34 << std::endl;
        stream << std::setw(10) << m.a41 << " " << std::setw(10) << m.a42 << " " << std::setw(10) << m.a43 << " " << std::setw(10) << m.a44;

        return stream;
    }

    //explicit template
    template class Matrix4<float>;
    template Matrix4<float> operator *<float>(const Matrix4<float>&, float);
    template Matrix4<float> operator *<float>(float, const Matrix4<float>&);
    template Matrix4<float> operator /<float>(const Matrix4<float>&, float);
    template std::ostream& operator <<<float>(std::ostream&, const Matrix4<float>&);

    template class Matrix4<double>;
    template Matrix4<double> operator *<double>(const Matrix4<double>&, double);
    template Matrix4<double> operator *<double>(double, const Matrix4<double>&);
    template Matrix4<double> operator /<double>(const Matrix4<double>&, double);
    template std::ostream& operator <<<double>(std::ostream&, const Matrix4<double>&);

}
