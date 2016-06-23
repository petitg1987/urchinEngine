#include "math/algebra/Transform.h"

namespace urchin
{
	
	template<class T> Transform<T>::Transform()
	{
		pPosition.setNull();
		qOrientation.setIdentity();
		fScale=1.0;

		//not need to compute transform matrix: it's the identity matrix
	}

	template<class T> Transform<T>::Transform(const Point3<T> &position, const Quaternion<T> &orientation, T scale) :
		pPosition(position),
		qOrientation(orientation),
		fScale(scale)
	{
		mPosition.buildTranslation(pPosition.X, pPosition.Y, pPosition.Z);
		mOrientation = qOrientation.toMatrix4();
		mScale.buildScale(fScale, fScale, fScale);
		mTransform = mPosition * mScale * mOrientation;
	}

	template<class T> void Transform<T>::setPosition(const Point3<T> &position)
	{
		pPosition = position;

		mPosition.buildTranslation(pPosition.X, pPosition.Y, pPosition.Z);
		mTransform = mPosition * mScale * mOrientation;
	}

	template<class T> const Point3<T> &Transform<T>::getPosition() const
	{
		return pPosition;
	}

	template<class T> void Transform<T>::setOrientation(const Quaternion<T> &orientation)
	{
		qOrientation = orientation;

		mOrientation = qOrientation.toMatrix4();
		mTransform =  mPosition * mScale * mOrientation;
	}

	template<class T> const Quaternion<T> &Transform<T>::getOrientation() const
	{
		return qOrientation;
	}

	template<class T> void Transform<T>::setScale(T scale)
	{
		fScale = scale;

		mScale.buildScale(fScale, fScale, fScale);
		mTransform = mPosition * mScale * mOrientation;
	}

	template<class T> T Transform<T>::getScale() const
	{
		return fScale;
	}


	template<class T> const Matrix4<T> &Transform<T>::getPositionMatrix() const
	{
		return mPosition;
	}

	template<class T> const Matrix4<T> &Transform<T>::getOrientationMatrix() const
	{
		return mOrientation;
	}

	template<class T> const Matrix4<T> &Transform<T>::getScaleMatrix() const
	{
		return mScale;
	}

	template<class T> const Matrix4<T> &Transform<T>::getTransformMatrix() const
	{
		return mTransform;
	}

	//explicit template
	template class Transform<float>;
	template class Transform<double>;

}
