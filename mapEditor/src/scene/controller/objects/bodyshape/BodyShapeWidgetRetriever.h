#ifndef URCHINENGINE_MAPEDITOR_BODYSHAPEWIDGETRETRIEVER_H
#define URCHINENGINE_MAPEDITOR_BODYSHAPEWIDGETRETRIEVER_H

#include <memory>
#include <QWidget>

#include "UrchinPhysicsEngine.h"
#include "UrchinMapHandler.h"
#include "scene/controller/objects/bodyshape/BodyShapeWidget.h"

namespace urchin
{

	class BodyShapeWidgetRetriever
	{
		public:
			BodyShapeWidgetRetriever(const SceneObject *);

			BodyShapeWidget *retrieveShapeWidget(std::shared_ptr<const CollisionShape3D>);
			BodyShapeWidget *retrieveShapeWidget(CollisionShape3D::ShapeType);

		private:
			const SceneObject *sceneObject;
	};

}

#endif
