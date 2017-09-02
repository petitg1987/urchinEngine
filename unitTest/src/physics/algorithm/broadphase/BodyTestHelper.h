#ifndef URCHINENGINE_BODYTESTHELPER_H
#define URCHINENGINE_BODYTESTHELPER_H

#include <memory>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"
using namespace urchin;

class BodyTestHelper
{
	public:
		static std::unique_ptr<WorkRigidBody> createCubeRigidBody(const Point3<float> &, float);

	private:
		BodyTestHelper();
		~BodyTestHelper();
};

#endif
