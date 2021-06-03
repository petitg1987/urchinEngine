#include <math/algebra/Transform.h>

namespace urchin {

    template<class T> Transform<T>::Transform() :
            fScale(1.0) {
        pPosition.setNull();
        qOrientation.setIdentity();
        //not need to compute transform matrix: it's the identity matrix
    }

    template<class T> Transform<T>::Transform(const Point3<T>& position, const Quaternion<T>& orientation, T scale) :
            pPosition(position),
            qOrientation(orientation),
            fScale(scale),
            mOrientation(orientation.toMatrix4()) {
        mPosition.buildTranslation(pPosition.X, pPosition.Y, pPosition.Z);

        mScale.buildScale(fScale, fScale, fScale);
        mTransform = mPosition * mScale * mOrientation;
    }

    template<class T> void Transform<T>::setPosition(const Point3<T>& position) {
        pPosition = position;

        mPosition.buildTranslation(pPosition.X, pPosition.Y, pPosition.Z);
        mTransform = mPosition * mScale * mOrientation;
    }

    template<class T> const Point3<T> &Transform<T>::getPosition() const {
        return pPosition;
    }

    template<class T> void Transform<T>::setOrientation(const Quaternion<T>& orientation) {
        qOrientation = orientation;

        mOrientation = qOrientation.toMatrix4();
        mTransform =  mPosition * mScale * mOrientation;
    }

    template<class T> const Quaternion<T> &Transform<T>::getOrientation() const {
        return qOrientation;
    }

    template<class T> void Transform<T>::setScale(T scale) {
        fScale = scale;

        mScale.buildScale(fScale, fScale, fScale);
        mTransform = mPosition * mScale * mOrientation;
    }

    template<class T> T Transform<T>::getScale() const {
        return fScale;
    }

    template<class T> const Matrix4<T> &Transform<T>::getPositionMatrix() const {
        return mPosition;
    }

    template<class T> const Matrix4<T> &Transform<T>::getOrientationMatrix() const {
        return mOrientation;
    }

    template<class T> const Matrix4<T> &Transform<T>::getScaleMatrix() const {
        return mScale;
    }

    template<class T> const Matrix4<T> &Transform<T>::getTransformMatrix() const {
        return mTransform;
    }

    template<class T> Transform<T> Transform<T>::operator *(const Transform<T>& transform) const {
        return Transform<T>(pPosition + qOrientation.rotatePoint(transform.getPosition()),
                         qOrientation * transform.getOrientation(),
                         fScale * transform.getScale());
    }

    template<class T> const Transform<T>& Transform<T>::operator *=(const Transform<T>& transform) {
        *this = *this * transform;

        return *this;
    }

    //explicit template
    template class Transform<float>;

    template class Transform<double>;

}
