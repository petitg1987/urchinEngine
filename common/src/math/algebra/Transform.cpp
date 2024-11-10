#include <math/algebra/Transform.h>
#include <math/algorithm/MathFunction.h>

namespace urchin {

    template<class T> Transform<T>::Transform() :
            vScale(Vector3<T>(1.0, 1.0, 1.0)) {
        pPosition.setNull();
        qOrientation.setIdentity();
        //not need to compute transform matrix: it's the identity matrix
    }

    template<class T> Transform<T>::Transform(const Point3<T>& position, const Quaternion<T>& orientation, const Vector3<T>& scale) :
            pPosition(position),
            qOrientation(orientation),
            vScale(scale),
            mPosition(Matrix4<T>::buildTranslation(pPosition.X, pPosition.Y, pPosition.Z)),
            mOrientation(orientation.toMatrix4()),
            mScale(Matrix4<T>::buildScale(vScale.X, vScale.Y, vScale.Z)),
            mTransform(mPosition * mOrientation * mScale) {

    }

    template<class T> void Transform<T>::setPosition(const Point3<T>& position) {
        pPosition = position;

        mPosition = Matrix4<T>::buildTranslation(pPosition.X, pPosition.Y, pPosition.Z);
        mTransform = mPosition * mOrientation * mScale;
    }

    template<class T> const Point3<T> &Transform<T>::getPosition() const {
        return pPosition;
    }

    template<class T> void Transform<T>::setOrientation(const Quaternion<T>& orientation) {
        qOrientation = orientation;

        mOrientation = qOrientation.toMatrix4();
        mTransform = mPosition * mOrientation * mScale;
    }

    template<class T> const Quaternion<T> &Transform<T>::getOrientation() const {
        return qOrientation;
    }

    template<class T> void Transform<T>::setScale(const Vector3<T>& scale) {
        vScale = scale;

        mScale = Matrix4<T>::buildScale(vScale.X, vScale.Y, vScale.Z);
        mTransform = mPosition * mOrientation * mScale;
    }

    template<class T> const Vector3<T>& Transform<T>::getScale() const {
        return vScale;
    }

    template<class T> bool Transform<T>::hasScaling() const {
        return !MathFunction::isOne((T)vScale.X, (T)0.001) || !MathFunction::isOne((T)vScale.Y, (T)0.001) || !MathFunction::isOne((T)vScale.Z, (T)0.001);
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
                         vScale * transform.getScale());
    }

    template<class T> const Transform<T>& Transform<T>::operator *=(const Transform<T>& transform) {
        *this = *this * transform;
        return *this;
    }

    template<class T> bool Transform<T>::operator ==(const Transform<T>& t) const {
        return pPosition == t.getPosition() && qOrientation == t.getOrientation() && vScale == t.getScale();
    }

    //explicit template
    template class Transform<float>;

    template class Transform<double>;

}
