#include "math/algebra/vector/Vector3.h"

namespace urchin
{
	
	template<class T> Vector3<T>::Vector3() : X(0), Y(0), Z(0)
	{

	}

	template<class T> Vector3<T>::Vector3(T Xu, T Yu, T Zu) : X(Xu), Y(Yu), Z(Zu)
	{

	}

	template<class T> Vector3<T>::Vector3(const Vector3<T> &vector) :
		X(vector.X), Y(vector.Y), Z(vector.Z)
	{

	}

	template<class T> Vector3<T>::Vector3(const Vector2<T> &v, T Zu) :
		X(v.X), Y(v.Y), Z(Zu)
	{

	}

	template<class T> void Vector3<T>::setValues(T Xu, T Yu, T Zu)
	{
		X = Xu;
		Y = Yu;
		Z = Zu;
	}

	template<class T> void Vector3<T>::setNull()
	{
		X = Y = Z = 0;
	}

	template<class T> Vector3<T> Vector3<T>::normalize() const
	{
		const T norm = (T)sqrt(X*X + Y*Y + Z*Z);
		if(norm > 0.0)
		{
			return Vector3<T>(X/norm, Y/norm, Z/norm);
		}

		return Vector3<T>(X, Y, Z);
	}

	template<class T> T Vector3<T>::length() const
	{
		return (T)sqrt(X*X + Y*Y + Z*Z);
	}

	template<class T> T Vector3<T>::squareLength() const
	{
		return (X*X + Y*Y + Z*Z);
	}

	template<class T> T Vector3<T>::dotProduct(const Vector3<T> &v) const
	{
		return (X*v.X + Y*v.Y + Z*v.Z);
	}

	template<class T> Vector3<T> Vector3<T>::crossProduct(const Vector3<T> &v) const
	{
		return Vector3<T>(	Y*v.Z - Z*v.Y,
					Z*v.X - X*v.Z,
					X*v.Y - Y*v.X);
	}

	template<class T> Vector3<T> Vector3<T>::operator +() const
	{
		return Vector3<T>(X, Y, Z);
	}

	template<class T> Vector3<T> Vector3<T>::operator -() const
	{
		return Vector3<T>(-X, -Y, -Z);
	}

	template<class T> Vector3<T> Vector3<T>::operator +(const Vector3<T> &v) const
	{
		return Vector3<T>(	X + v.X,
					Y + v.Y,
					Z + v.Z);
	}

	template<class T> Vector3<T> Vector3<T>::operator -(const Vector3<T> &v) const
	{
		return Vector3<T>(	X - v.X,
					Y - v.Y,
					Z - v.Z);
	}

	template<class T> Vector3<T> Vector3<T>::operator *(const Vector3<T> &v) const
	{
		return Vector3<T>(	X * v.X,
				Y * v.Y,
				Z * v.Z);
	}

	template<class T> Vector3<T> Vector3<T>::operator /(const Vector3<T> &v) const
	{
		return Vector3<T>(	X / v.X,
				Y / v.Y,
				Z / v.Z);
	}

	template<class T> const Vector3<T>& Vector3<T>::operator +=(const Vector3<T> &v)
	{
		X += v.X;
		Y += v.Y;
		Z += v.Z;

		return *this;
	}

	template<class T> const Vector3<T>& Vector3<T>::operator -=(const Vector3<T> &v)
	{
		X -= v.X;
		Y -= v.Y;
		Z -= v.Z;

		return *this;
	}

	template<class T> const Vector3<T>& Vector3<T>::operator *=(const Vector3<T> &v)
	{
		X *= v.X;
		Y *= v.Y;
		Z *= v.Z;

		return *this;
	}

	template<class T> const Vector3<T>& Vector3<T>::operator /=(const Vector3<T> &v)
	{
		X /= v.X;
		Y /= v.Y;
		Z /= v.Z;

		return *this;
	}

	template<class T> const Vector3<T>& Vector3<T>::operator *=(T t)
	{
		X *= t;
		Y *= t;
		Z *= t;

		return *this;
	}

	template<class T> const Vector3<T>& Vector3<T>::operator /=(T t)
	{
		X /= t;
		Y /= t;
		Z /= t;

		return *this;
	}

	template<class T> T& Vector3<T>::operator [](int i)
	{
		return (&X)[i];
	}

	template<class T> const T& Vector3<T>::operator [](int i) const
	{
		return (&X)[i];
	}

	template<class T> Vector3<T>::operator T*()
	{
		return &X;
	}

	template<class T> Vector3<T>::operator const T*() const
	{
		return &X;
	}

	template<class T> template<class NEW_TYPE> Vector3<NEW_TYPE> Vector3<T>::cast() const
	{
		return Vector3<NEW_TYPE>((NEW_TYPE)X, (NEW_TYPE)Y, (NEW_TYPE)Z);
	}

	template<class T> Vector3<T> operator *(const Vector3<T> &v, T t)
	{
		return Vector3<T>(v.X * t, v.Y * t, v.Z * t);
	}

	template<class T> Vector3<T> operator *(T t, const Vector3<T> &v)
	{
		return v * t;
	}

	template<class T> Vector3<T> operator /(const Vector3<T> &v, T t)
	{
		return Vector3<T>(v.X / t, v.Y / t, v.Z / t);
	}

	template<class T> Vector3<T> operator *(const Matrix3<T> &m, const Vector3<T> &v)
	{
		return Vector3<T>(	m.a11 * v.X + m.a12 * v.Y + m.a13 * v.Z,
					m.a21 * v.X + m.a22 * v.Y + m.a23 * v.Z,
					m.a31 * v.X + m.a32 * v.Y + m.a33 * v.Z);
	}

	template<class T> Vector3<T> operator *(const Vector3<T> &v, const Matrix3<T> &m)
	{
		return m * v;
	}

	template<class T> std::ostream& operator <<(std::ostream &stream, const Vector3<T> &v)
	{
		return stream << v.X << " " << v.Y << " " << v.Z;
	}

	//explicit template
	template class Vector3<float>;
	template Vector3<float> Vector3<float>::cast() const;
	template Vector3<double> Vector3<float>::cast() const;
	template Vector3<float> operator *<float>(const Vector3<float> &, float);
	template Vector3<float> operator *<float>(float, const Vector3<float> &);
	template Vector3<float> operator /<float>(const Vector3<float> &, float);
	template Vector3<float> operator *<float>(const Matrix3<float> &, const Vector3<float> &);
	template Vector3<float> operator *<float>(const Vector3<float> &, const Matrix3<float> &);
	template std::ostream& operator <<<float>(std::ostream &, const Vector3<float> &);

	template class Vector3<double>;
	template Vector3<double> Vector3<double>::cast() const;
	template Vector3<float> Vector3<double>::cast() const;
	template Vector3<double> operator *<double>(const Vector3<double> &, double);
	template Vector3<double> operator *<double>(double, const Vector3<double> &);
	template Vector3<double> operator /<double>(const Vector3<double> &, double);
	template Vector3<double> operator *<double>(const Matrix3<double> &, const Vector3<double> &);
	template Vector3<double> operator *<double>(const Vector3<double> &, const Matrix3<double> &);
	template std::ostream& operator <<<double>(std::ostream &, const Vector3<double> &);

}
