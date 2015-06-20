#include <stdexcept>

#include "BodyShapeWidgetRetriever.h"
#include "scene/controller/objects/bodyshape/BodyShapeWidget.h"
#include "scene/controller/objects/bodyshape/NoBodyShapeWidget.h"
#include "scene/controller/objects/bodyshape/BodySphereShapeWidget.h"
#include "scene/controller/objects/bodyshape/BodyBoxShapeWidget.h"
#include "scene/controller/objects/bodyshape/BodyCapsuleShapeWidget.h"
#include "scene/controller/objects/bodyshape/BodyCylinderShapeWidget.h"
#include "scene/controller/objects/bodyshape/BodyConvexHullShapeWidget.h"
#include "scene/controller/objects/bodyshape/BodyCompoundShapeWidget.h"

namespace urchin
{

	BodyShapeWidgetRetriever::BodyShapeWidgetRetriever(QWidget *parentWidget, const SceneObject *sceneObject) :
			parentWidget(parentWidget),
			sceneObject(sceneObject)
	{

	}

	BodyShapeWidget *BodyShapeWidgetRetriever::retrieveShapeWidget(std::shared_ptr<const CollisionShape3D> shape)
	{
		if(shape!=nullptr)
		{
			return retrieveShapeWidget(shape->getShapeType());
		}

		return new NoBodyShapeWidget(parentWidget, sceneObject);
	}

	BodyShapeWidget *BodyShapeWidgetRetriever::retrieveShapeWidget(CollisionShape3D::ShapeType shapeType)
	{
		BodyShapeWidget *bodyShapeWidget;

		if(shapeType==CollisionShape3D::ShapeType::SPHERE_SHAPE)
		{
			bodyShapeWidget = new BodySphereShapeWidget(parentWidget, sceneObject);
		}else if(shapeType==CollisionShape3D::ShapeType::BOX_SHAPE)
		{
			bodyShapeWidget = new BodyBoxShapeWidget(parentWidget, sceneObject);
		}else if(shapeType==CollisionShape3D::ShapeType::CAPSULE_SHAPE)
		{
			bodyShapeWidget = new BodyCapsuleShapeWidget(parentWidget, sceneObject);
		}else if(shapeType==CollisionShape3D::ShapeType::CYLINDER_SHAPE)
		{
			bodyShapeWidget = new BodyCylinderShapeWidget(parentWidget, sceneObject);
		}else if(shapeType==CollisionShape3D::ShapeType::CONVEX_HULL_SHAPE)
		{
			bodyShapeWidget = new BodyConvexHullShapeWidget(parentWidget, sceneObject);
		}else if(shapeType==CollisionShape3D::ShapeType::COMPOUND_SHAPE)
		{
			bodyShapeWidget = new BodyCompoundShapeWidget(parentWidget, sceneObject);
		}else
		{
			throw new std::invalid_argument("Unknown shape type to retrieve body shape widget: " + shapeType);
		}

		return bodyShapeWidget;
	}

}
