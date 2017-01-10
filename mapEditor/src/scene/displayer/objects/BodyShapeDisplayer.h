#ifndef ENGINE_MAPEDITOR_BODYSHAPEDISPLAYER_H
#define ENGINE_MAPEDITOR_BODYSHAPEDISPLAYER_H

#include "Urchin3dEngine.h"
#include "UrchinMapHandler.h"

namespace urchin
{

	class BodyShapeDisplayer
	{
		public:
			BodyShapeDisplayer(SceneManager *);
			~BodyShapeDisplayer();

			void setSelectedCompoundShapeComponent(std::shared_ptr<const LocalizedCollisionShape>);

			void displayBodyShapeFor(const SceneObject *);

		private:
			GeometryModel *retrieveSingleGeometry(CollisionShape3D::ShapeType shapeType, const CollisionConvexObject3D *);

			GeometryModel *retrieveSphereGeometry(const CollisionConvexObject3D *) const;
			GeometryModel *retrieveBoxGeometry(const CollisionConvexObject3D *) const;
			GeometryModel *retrieveCylinderGeometry(const CollisionConvexObject3D *) const;
			GeometryModel *retrieveConeGeometry(const CollisionConvexObject3D *) const;
			GeometryModel *retrieveCapsuleGeometry(const CollisionConvexObject3D *) const;
			GeometryModel *retrieveConvexHullGeometry(const CollisionConvexObject3D *) const;

			void cleanCurrentDisplay();

			SceneManager *sceneManager;
			std::vector<GeometryModel *> bodyShapeModels;

			std::shared_ptr<const LocalizedCollisionShape> selectedCompoundShapeComponent;
	};

}

#endif
