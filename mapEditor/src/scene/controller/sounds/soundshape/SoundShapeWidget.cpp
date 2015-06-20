#include <stdexcept>

#include "SoundShapeWidget.h"

namespace urchin
{

	SoundShapeWidget::SoundShapeWidget(QWidget *parent, const SceneSound *sceneSound) :
			QWidget(parent),
			disableShapeEvent(false),
			sceneSound(sceneSound),
			shape(nullptr)
	{

	}

	SoundShapeWidget::~SoundShapeWidget()
	{

	}

	const SceneSound *SoundShapeWidget::getSceneSound() const
	{
		return sceneSound;
	}

	const SoundShape *SoundShapeWidget::retrieveShape()
	{
		if(shape==nullptr)
		{
			shape = createSoundShape();
		}
		return shape;
	}

	void SoundShapeWidget::setupShapePropertiesFrom(const SoundShape *shape)
	{
		disableShapeEvent = true;

		doSetupShapePropertiesFrom(shape);

		disableShapeEvent = false;
	}

	void SoundShapeWidget::updateSoundShape()
	{
		if(!disableShapeEvent)
		{
			shape = createSoundShape();

			emit soundShapeChange(shape);
		}
	}
}
