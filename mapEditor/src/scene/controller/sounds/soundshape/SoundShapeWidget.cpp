#include <stdexcept>
#include <QtWidgets/QLabel>

#include "SoundShapeWidget.h"
#include "support/SpinBoxStyleHelper.h"

namespace urchin
{

	SoundShapeWidget::SoundShapeWidget(QWidget *parent, const SceneSound *sceneSound) :
			QWidget(parent),
			disableShapeEvent(false),
			sceneSound(sceneSound),
			shape(nullptr)
	{
		QLabel *marginLabel = new QLabel(this);
		marginLabel->setText("Margin:");
		marginLabel->setGeometry(QRect(5, 0, 80, 22));

		margin = new QDoubleSpinBox(this);
		margin->setGeometry(QRect(85, 0, 80, 22));
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
