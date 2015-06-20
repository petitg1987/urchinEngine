#include "body/InertiaCalculation.h"

namespace urchin
{

	InertiaCalculation::InertiaCalculation()
	{

	}

	InertiaCalculation::~InertiaCalculation()
	{

	}

	Matrix3<float> InertiaCalculation::computeInverseWorldInertia(const Vector3<float> &invLocalInertia, const PhysicsTransform &physicsTransform)
	{
		//following source code is equivalent to: worldMatrix * toMatrix(invLocalInertia) * worldMatrix.inverse()
		const Matrix3<float> orientationMultiplyInertia = physicsTransform.getOrientationMatrix().scaled(invLocalInertia.X, invLocalInertia.Y, invLocalInertia.Z);

		return orientationMultiplyInertia * physicsTransform.getOrientationMatrix().transpose();
	}

}
