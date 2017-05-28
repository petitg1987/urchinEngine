#ifndef ENGINE_AIWORLDGENERATOR_H
#define ENGINE_AIWORLDGENERATOR_H

#include <memory>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"
#include "UrchinPhysicsEngine.h"

#include "resources/Map.h"

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
			std::shared_ptr<AIWorld> generate(const Map *);

		private:
			std::vector<LocalizedShape> extractionLocalizedShapes(RigidBody *);
	};

}

#endif
