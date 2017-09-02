#ifndef URCHINENGINE_AIWORLDGENERATOR_H
#define URCHINENGINE_AIWORLDGENERATOR_H

#include <memory>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"
#include "UrchinPhysicsEngine.h"

#include "resources/object/SceneObject.h"

namespace urchin
{

	struct LocalizedShape
	{
		std::shared_ptr<const ConvexShape3D<float>> shape;
		Transform<float> worldTransform;
	};

	class AIWorldGenerator
	{
		public:
			std::shared_ptr<AIWorld> generate(const std::list<SceneObject *> &);

		private:
			std::vector<LocalizedShape> extractionLocalizedShapes(RigidBody *);
	};

}

#endif
