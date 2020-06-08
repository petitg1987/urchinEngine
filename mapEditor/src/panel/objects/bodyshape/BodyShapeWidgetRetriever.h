#ifndef URCHINENGINE_BODYSHAPEWIDGETRETRIEVER_H
#define URCHINENGINE_BODYSHAPEWIDGETRETRIEVER_H

#include <memory>
#include <QWidget>

#include "UrchinPhysicsEngine.h"
#include "UrchinMapHandler.h"
#include "panel/objects/bodyshape/BodyShapeWidget.h"

namespace urchin
{

	class BodyShapeWidgetRetriever
	{
		public:
			explicit BodyShapeWidgetRetriever(const SceneObject *);

			BodyShapeWidget *createBodyShapeWidget(const std::shared_ptr<const CollisionShape3D>&);
			BodyShapeWidget *createBodyShapeWidget(CollisionShape3D::ShapeType);

		private:
			const SceneObject *sceneObject;
	};

}

#endif
