#include <QtWidgets/QLabel>

#include "BodyConeShapeWidget.h"
#include "widget/style/SpinBoxStyleHelper.h"

namespace urchin
{

	BodyConeShapeWidget::BodyConeShapeWidget(const SceneObject *sceneObject) :
			BodyShapeWidget(sceneObject)
	{
        auto *radiusLabel = new QLabel("Radius:");
		mainLayout->addWidget(radiusLabel, 0, 0);

		radius = new QDoubleSpinBox();
		mainLayout->addWidget(radius, 0, 1);
		SpinBoxStyleHelper::applyDefaultStyleOn(radius);
		radius->setMinimum(0.0);
		connect(radius, SIGNAL(valueChanged(double)), this, SLOT(updateBodyShape()));

        auto *heightLabel = new QLabel("Height:");
		mainLayout->addWidget(heightLabel, 1, 0);

		height = new QDoubleSpinBox();
		mainLayout->addWidget(height, 1, 1);
		SpinBoxStyleHelper::applyDefaultStyleOn(height);
		height->setMinimum(0.0);
		connect(height, SIGNAL(valueChanged(double)), this, SLOT(updateBodyShape()));

        auto *orientationLabel = new QLabel("Orientation:");
		mainLayout->addWidget(orientationLabel, 2, 0);

		orientation = new QComboBox();
		mainLayout->addWidget(orientation, 2, 1);
		orientation->addItem("X Positive", QVariant(ConeShape<float>::ConeOrientation::CONE_X_POSITIVE));
		orientation->addItem("X Negative", QVariant(ConeShape<float>::ConeOrientation::CONE_X_NEGATIVE));
		orientation->addItem("Y Positive", QVariant(ConeShape<float>::ConeOrientation::CONE_Y_POSITIVE));
		orientation->addItem("Y Negative", QVariant(ConeShape<float>::ConeOrientation::CONE_Y_NEGATIVE));
		orientation->addItem("Z Positive", QVariant(ConeShape<float>::ConeOrientation::CONE_Z_POSITIVE));
		orientation->addItem("Z Negative", QVariant(ConeShape<float>::ConeOrientation::CONE_Z_NEGATIVE));
		connect(orientation, SIGNAL(currentIndexChanged(int)), this, SLOT(updateBodyShape()));
	}

	std::string BodyConeShapeWidget::getBodyShapeName() const
	{
		return CONE_SHAPE_LABEL;
	}

	void BodyConeShapeWidget::doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D> shape)
	{
		const auto *coneShape = dynamic_cast<const CollisionConeShape *>(shape.get());

		radius->setValue(coneShape->getRadius());
		height->setValue(coneShape->getHeight());

		int index = orientation->findData(coneShape->getConeOrientation());
		if(index!=-1)
		{
			orientation->setCurrentIndex(index);
		}
	}

	std::shared_ptr<const CollisionShape3D> BodyConeShapeWidget::createBodyShape() const
	{
		QVariant variant = orientation->currentData();
		auto orientation = static_cast<ConeShape<float>::ConeOrientation>(variant.toInt());

		return std::make_shared<const CollisionConeShape>(radius->value(), height->value(), orientation);
	}

}
