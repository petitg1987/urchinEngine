#include "math/algebra/matrix/Matrix3.h"

namespace urchin
{

	/**
	 * Construct an identify matrix
	 */
	template<class T> Matrix3<T>::Matrix3()
	{
		a11=1.0; a12=0.0; a13=0.0;
		a21=0.0; a22=1.0; a23=0.0;
		a31=0.0; a32=0.0; a33=1.0;
	}

	template<class T> Matrix3<T>::Matrix3(T m11, T m12, T m13,
		T m21, T m22, T m23,
		T m31, T m32, T m33)
	{
		a11=m11; a12=m12; a13=m13;
		a21=m21; a22=m22; a23=m23;
		a31=m31; a32=m32; a33=m33;
	}

	template<class T> void Matrix3<T>::setValues(T m11, T m12, T m13,
		T m21, T m22, T m23,
		T m31, T m32, T m33)
	{
		a11=m11; a12=m12; a13=m13;
		a21=m21; a22=m22; a23=m23;
		a31=m31; a32=m32; a33=m33;
	}

	template<class T> void Matrix3<T>::setIdentity()
	{
		a11 = 1.0; a12 = 0.0; a13 = 0.0;
		a21 = 0.0; a22 = 1.0; a23 = 0.0;
		a31 = 0.0; a32 = 0.0; a33 = 1.0;
	}

	template<class T> T Matrix3<T>::determinant() const
	{
		return a11 * (a22 * a33 - a23 * a32) - a21 * (a12 * a33 - a13 * a32) + a31 * (a12 * a23 - a13 * a22);
	}

	template<class T> Matrix3<T> Matrix3<T>::transpose() const
	{
		return Matrix3<T>(	a11, a21, a31,
				a12, a22, a32,
				a13, a23, a33);
	}

	template<class T> Matrix3<T> Matrix3<T>::inverse() const
	{
		Matrix3<T> ret;
		T det = determinant();

		if (std::fabs(det) > 0.0)
		{
			ret.a11 =  (a22 * a33 - a32 * a23) / det;
			ret.a12 = -(a12 * a33 - a32 * a13) / det;
			ret.a13 =  (a12 * a23 - a13 * a22) / det;

			ret.a21 = -(a21 * a33 - a23 * a31) / det;
			ret.a22 =  (a11 * a33 - a13 * a31) / det;
			ret.a23 = -(a11 * a23 - a13 * a21) / det;

			ret.a31 =  (a21 * a32 - a22 * a31) / det;
			ret.a32 = -(a11 * a32 - a12 * a31) / det;
			ret.a33 =  (a11 * a22 - a21 * a12) / det;
		}

		return ret;
	}

	template<class T> Matrix3<T> Matrix3<T>::scaled(T scaleFactorColumn0, T scaleFactorColumn1, T scaleFactorColumn2) const
	{
		return Matrix3<T>(a11*scaleFactorColumn0, a12*scaleFactorColumn1, a13*scaleFactorColumn2,
				a21*scaleFactorColumn0, a22*scaleFactorColumn1, a23*scaleFactorColumn2,
				a31*scaleFactorColumn0, a32*scaleFactorColumn1, a33*scaleFactorColumn2);
	}

	template<class T> void Matrix3<T>::buildTranslation(T x, T y)
	{
		a11 = 1.0; a12 = 0.0; a13 = x;
		a21 = 0.0; a22 = 1.0; a23 = y;
		a31 = 0.0; a32 = 0.0; a33 = 1.0;
	}

	template<class T> void Matrix3<T>::buildScale(T x, T y, T z)
	{
		a11 = x;    a12 = 0.0; a13 = 0.0;
		a21 = 0.0; a22 = y;    a23 = 0.0;
		a31 = 0.0; a32 = 0.0; a33 = z;
	}

	template<class T> void Matrix3<T>::buildRotationX(T angle)
	{
		const T cos = std::cos(angle);
		const T sin = std::sin(angle);

		a11 = 1.0; a12 = 0.0; a13 = 0.0;
		a21 = 0.0; a22 = cos;  a23 = -sin;
		a31 = 0.0; a32 = sin;  a33 = cos;
	}

	template<class T> void Matrix3<T>::buildRotationY(T angle)
	{
		const T cos = std::cos(angle);
		const T sin = std::sin(angle);

		a11 = cos;  a12 = 0.0; a13 = sin;
		a21 = 0.0; a22 = 1.0; a23 = 0.0;
		a31 = -sin;  a32 = 0.0; a33 = cos;
	}

	template<class T> void Matrix3<T>::buildRotationZ(T angle)
	{
		const T cos = std::cos(angle);
		const T sin = std::sin(angle);

		a11 = cos;  a12 = -sin;  a13 = 0.0;
		a21 = sin;  a22 = cos;  a23 = 0.0;
		a31 = 0.0; a32 = 0.0; a33 = 1.0;
	}

	template<class T> Matrix3<T> Matrix3<T>::operator +() const
	{
		return *this;
	}

	template<class T> Matrix3<T> Matrix3<T>::operator -() const
	{
		return Matrix3<T>(	-a11, -a12, -a13,
				-a21, -a22, -a23,
				-a31, -a32, -a33);
	}

	template<class T> Matrix3<T> Matrix3<T>::operator +(const Matrix3<T>& m) const
	{
		return Matrix3<T>(	a11 + m.a11, a12 + m.a12, a13 + m.a13,
				a21 + m.a21, a22 + m.a22, a23 + m.a23,
				a31 + m.a31, a32 + m.a32, a33 + m.a33);
	}

	template<class T> Matrix3<T> Matrix3<T>::operator -(const Matrix3& m) const
	{
		return Matrix3(	a11 - m.a11, a12 - m.a12, a13 - m.a13,
				a21 - m.a21, a22 - m.a22, a23 - m.a23,
				a31 - m.a31, a32 - m.a32, a33 - m.a33);
	}

	template<class T> const Matrix3<T>& Matrix3<T>::operator +=(const Matrix3<T>& m)
	{
		a11 += m.a11; a12 += m.a12; a13 += m.a13;
		a21 += m.a21; a22 += m.a22; a23 += m.a23;
		a31 += m.a31; a32 += m.a32; a33 += m.a33;

		return *this;
	}

	template<class T> const Matrix3<T>& Matrix3<T>::operator -=(const Matrix3<T>& m)
	{
		a11 -= m.a11; a12 -= m.a12; a13 -= m.a13;
		a21 -= m.a21; a22 -= m.a22; a23 -= m.a23;
		a31 -= m.a31; a32 -= m.a32; a33 -= m.a33;

		return *this;
	}

	template<class T> Matrix3<T> Matrix3<T>::operator *(const Matrix3<T>& m) const
	{
		return Matrix3<T>(
				a11 * m.a11 + a12 * m.a21 + a13 * m.a31,
				a11 * m.a12 + a12 * m.a22 + a13 * m.a32,
				a11 * m.a13 + a12 * m.a23 + a13 * m.a33,

				a21 * m.a11 + a22 * m.a21 + a23 * m.a31,
				a21 * m.a12 + a22 * m.a22 + a23 * m.a32,
				a21 * m.a13 + a22 * m.a23 + a23 * m.a33,

				a31 * m.a11 + a32 * m.a21 + a33 * m.a31,
				a31 * m.a12 + a32 * m.a22 + a33 * m.a32,
				a31 * m.a13 + a32 * m.a23 + a33 * m.a33);
	}

	template<class T> const Matrix3<T>& Matrix3<T>::operator *=(const Matrix3<T>& m)
	{
		*this = *this * m;

		return *this;
	}

	template<class T> const Matrix3<T>& Matrix3<T>::operator *=(T t)
	{
		a11 *= t; a12 *= t; a13 *= t;
		a21 *= t; a22 *= t; a23 *= t;
		a31 *= t; a32 *= t; a33 *= t;

		return *this;
	}

	template<class T> const Matrix3<T>& Matrix3<T>::operator /=(T t)
	{
		a11 /= t; a12 /= t; a13 /= t;
		a21 /= t; a22 /= t; a23 /= t;
		a31 /= t; a32 /= t; a33 /= t;

		return *this;
	}

	template<class T> T& Matrix3<T>::operator ()(size_t line, size_t column)
	{
		return (&a11)[column * 3 + line];
	}

	template<class T> const T& Matrix3<T>::operator ()(size_t line, size_t column) const
	{
		return (&a11)[column * 3 + line];
	}

	template<class T> Matrix3<T>::operator T*()
	{
		return &a11;
	}

	template<class T> Matrix3<T>::operator const T*() const
	{
		return &a11;
	}

	template<class T> Matrix3<T> operator *(const Matrix3<T> &m, T t)
	{
		return Matrix3<T>(	m.a11 * t, m.a12 * t, m.a13 * t,
				m.a21 * t, m.a22 * t, m.a23 * t,
				m.a31 * t, m.a32 * t, m.a33 * t);
	}

	template<class T> Matrix3<T> operator *(T t, const Matrix3<T> &m)
	{
		return m * t;
	}

	template<class T> Matrix3<T> operator /(const Matrix3<T> &m, T t)
	{
		return Matrix3<T>(	m.a11 / t, m.a12 / t, m.a13 / t,
				m.a21 / t, m.a22 / t, m.a23 / t,
				m.a31 / t, m.a32 / t, m.a33 / t);
	}

	template<class T> std::ostream& operator <<(std::ostream &stream, const Matrix3<T> &m)
	{
		stream.setf(std::ios::left);
		stream << std::setw(10) << m.a11 << " " << std::setw(10) << m.a12 << " " << std::setw(10) << m.a13 << std::endl;
		stream << std::setw(10) << m.a21 << " " << std::setw(10) << m.a22 << " " << std::setw(10) << m.a23 << std::endl;
		stream << std::setw(10) << m.a31 << " " << std::setw(10) << m.a32 << " " << std::setw(10) << m.a33 << std::endl;

		return stream;
	}

	//explicit template
	template class Matrix3<float>;
	template Matrix3<float> operator *<float>(const Matrix3<float> &m, float);
	template Matrix3<float> operator *<float>(float, const Matrix3<float> &);
	template Matrix3<float> operator /<float>(const Matrix3<float> &, float);
	template std::ostream& operator <<<float>(std::ostream &, const Matrix3<float> &);

	template class Matrix3<double>;
	template Matrix3<double> operator *<double>(const Matrix3<double> &m, double);
	template Matrix3<double> operator *<double>(double, const Matrix3<double> &);
	template Matrix3<double> operator /<double>(const Matrix3<double> &, double);
	template std::ostream& operator <<<double>(std::ostream &, const Matrix3<double> &);

}
