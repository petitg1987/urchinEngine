#include <stdexcept>

#include "SoundShapeWidgetRetriever.h"
#include "scene/controller/sounds/soundshape/SoundShapeWidget.h"
#include "scene/controller/sounds/soundshape/SoundSphereShapeWidget.h"
#include "scene/controller/sounds/soundshape/SoundBoxShapeWidget.h"

namespace urchin
{

	SoundShapeWidgetRetriever::SoundShapeWidgetRetriever(const SceneSound *sceneSound) :
			sceneSound(sceneSound)
	{

	}

	SoundShapeWidget *SoundShapeWidgetRetriever::retrieveShapeWidget(const SoundShape *shape)
	{
		if(!shape)
		{
			throw std::invalid_argument("Shape used to retrieve sound shape widget cannot be null");
		}

		return retrieveShapeWidget(shape->getShapeType());
	}

	SoundShapeWidget *SoundShapeWidgetRetriever::retrieveShapeWidget(SoundShape::ShapeType shapeType)
	{
		SoundShapeWidget *soundShapeWidget;

		if(shapeType==SoundShape::ShapeType::SPHERE_SHAPE)
		{
			soundShapeWidget = new SoundSphereShapeWidget(sceneSound);
		}else if(shapeType==SoundShape::ShapeType::BOX_SHAPE)
		{
			soundShapeWidget = new SoundBoxShapeWidget(sceneSound);
		}else
		{
			throw std::invalid_argument("Unknown shape type to retrieve sound shape widget: " + std::to_string(shapeType));
		}

		return soundShapeWidget;
	}

}
