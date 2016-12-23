#include <stdexcept>
#include <QtWidgets/QLabel>

#include "SoundShapeWidget.h"
#include "support/SpinBoxStyleHelper.h"

namespace urchin
{

	SoundShapeWidget::SoundShapeWidget(const SceneSound *sceneSound) :
			disableShapeEvent(false),
			sceneSound(sceneSound),
			shape(nullptr)
	{
		setContentsMargins(0, 0, 0, 0);

		mainLayout = new QGridLayout(this);
		mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignTop);
		mainLayout->setContentsMargins(0, 0, 0, 0);

		QLabel *marginLabel = new QLabel("Margin:", this);
		mainLayout->addWidget(marginLabel, 0, 0);

		margin = new QDoubleSpinBox(this);
		mainLayout->addWidget(margin, 0, 1);
		SpinBoxStyleHelper::applyDefaultStyleOn(margin);
		margin->setMinimum(0.0);
		connect(margin, SIGNAL(valueChanged(double)), this, SLOT(updateSoundShape()));
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

		margin->setValue(shape->getMargin());

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

	float SoundShapeWidget::getMarginValue() const
	{
		return margin->value();
	}
}
