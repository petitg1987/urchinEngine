#include <body/InertiaCalculation.h>

namespace urchin {

    Matrix3<float> InertiaCalculation::computeInverseWorldInertia(const Vector3<float>& invLocalInertia, const PhysicsTransform& physicsTransform) {
        //following source code is equivalent to: worldMatrix * toMatrix(invLocalInertia) * worldMatrix.inverse()
        Matrix3<float> transformOrientation = physicsTransform.retrieveOrientationMatrix();
        const Matrix3<float> orientationMultiplyInertia = transformOrientation.scaled(invLocalInertia.X, invLocalInertia.Y, invLocalInertia.Z);

        return orientationMultiplyInertia * transformOrientation.transpose();
    }

}
