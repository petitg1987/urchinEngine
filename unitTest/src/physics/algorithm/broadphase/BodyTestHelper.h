#ifndef ENGINE_BODYTESTHELPER_H
#define ENGINE_BODYTESTHELPER_H

#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"
using namespace urchin;

class BodyTestHelper
{
	public:
		static WorkRigidBody *createCubeRigidBody(const Point3<float> &, float);

	private:
		BodyTestHelper();
		~BodyTestHelper();
};

#endif
