#ifndef ENGINE_MAPEDITOR_DEFAULTBODYSHAPECREATOR_H
#define ENGINE_MAPEDITOR_DEFAULTBODYSHAPECREATOR_H

#include <memory>

#include "UrchinMapHandler.h"
#include "UrchinPhysicsEngine.h"

namespace urchin
{

	class DefaultBodyShapeCreator
	{

		public:
			DefaultBodyShapeCreator(const SceneObject *);

			std::shared_ptr<const CollisionShape3D> createDefaultBodyShape(CollisionShape3D::ShapeType, bool) const;

		private:
			const SceneObject *sceneObject;
	};

}

#endif
