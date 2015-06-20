#include <stdexcept>

#include "SoundShapeWidgetRetriever.h"
#include "scene/controller/sounds/soundshape/SoundShapeWidget.h"
#include "scene/controller/sounds/soundshape/SoundSphereShapeWidget.h"
#include "scene/controller/sounds/soundshape/SoundBoxShapeWidget.h"

namespace urchin
{

	SoundShapeWidgetRetriever::SoundShapeWidgetRetriever(QWidget *parentWidget, const SceneSound *sceneSound) :
			parentWidget(parentWidget),
			sceneSound(sceneSound)
	{

	}

	SoundShapeWidget *SoundShapeWidgetRetriever::retrieveShapeWidget(const SoundShape *shape)
	{
		if(shape==nullptr)
		{
			throw new std::invalid_argument("Shape used to retrieve sound shape widget cannot be null");
		}

		return retrieveShapeWidget(shape->getShapeType());
	}

	SoundShapeWidget *SoundShapeWidgetRetriever::retrieveShapeWidget(SoundShape::ShapeType shapeType)
	{
		SoundShapeWidget *soundShapeWidget;

		if(shapeType==SoundShape::ShapeType::SPHERE_SHAPE)
		{
			soundShapeWidget = new SoundSphereShapeWidget(parentWidget, sceneSound);
		}else if(shapeType==SoundShape::ShapeType::BOX_SHAPE)
		{
			soundShapeWidget = new SoundBoxShapeWidget(parentWidget, sceneSound);
		}else
		{
			throw new std::invalid_argument("Unknown shape type to retrieve sound shape widget: " + shapeType);
		}

		return soundShapeWidget;
	}

}
