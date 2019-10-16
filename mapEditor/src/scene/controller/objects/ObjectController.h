#ifndef URCHINENGINE_MAPEDITOR_OBJECTCONTROLLER_H
#define URCHINENGINE_MAPEDITOR_OBJECTCONTROLLER_H

#include <list>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"
#include "UrchinMapHandler.h"

namespace urchin
{

	class ObjectController
	{
		public:
			explicit ObjectController(MapHandler *);

			bool isModified() const;
			void resetModified();

			std::list<const SceneObject *> getSceneObjects() const;
			const SceneObject *findSceneObjectByBodyId(const std::string &) const;
			void addSceneObject(SceneObject *);
			void removeSceneObject(const SceneObject *);
			void cloneSceneObject(SceneObject *, const SceneObject *);

			void createDefaultBody(const SceneObject *);
			void changeBodyShape(const SceneObject *, CollisionShape3D::ShapeType);
			void removeBody(const SceneObject *);

			const SceneObject *updateSceneObjectTransform(const SceneObject *, const Transform<float> &);
			const SceneObject *updateSceneObjectFlags(const SceneObject *, bool);
			const SceneObject *updateSceneObjectPhysicsProperties(const SceneObject *, float, float, float, float, float, float,
					const Vector3<float> &, const Vector3<float> &);
			const SceneObject *updateSceneObjectPhysicsShape(const SceneObject *, const std::shared_ptr< const CollisionShape3D>&);

		private:
			void markModified();
			SceneObject *findSceneObject(const SceneObject *);

			bool bIsModified;
			MapHandler *mapHandler;
	};

}

#endif
