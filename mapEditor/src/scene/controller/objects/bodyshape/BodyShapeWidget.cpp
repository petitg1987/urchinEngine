#include <stdexcept>

#include "BodyShapeWidget.h"

namespace urchin
{

	BodyShapeWidget::BodyShapeWidget(QWidget *parent, const SceneObject *sceneObject) :
			QWidget(parent),
			disableShapeEvent(false),
			sceneObject(sceneObject)
	{

	}

	BodyShapeWidget::~BodyShapeWidget()
	{

	}

	const SceneObject *BodyShapeWidget::getSceneObject() const
	{
		return sceneObject;
	}

	std::shared_ptr<const CollisionShape3D> BodyShapeWidget::retrieveShape()
	{
		if(shape.get()==nullptr)
		{
			shape = createBodyShape();
		}
		return shape;
	}

	void BodyShapeWidget::setupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D> shape)
	{
		disableShapeEvent = true;

		doSetupShapePropertiesFrom(shape);

		disableShapeEvent = false;
	}

	void BodyShapeWidget::updateBodyShape()
	{
		if(!disableShapeEvent)
		{
			shape = createBodyShape();

			emit bodyShapeChange(shape);
		}
	}
}
