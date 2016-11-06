#include <stdexcept>

#include "BodyCompoundShapeWidget.h"
#include "support/LabelStyleHelper.h"
#include "support/GroupBoxStyleHelper.h"
#include "support/SpinBoxStyleHelper.h"
#include "support/ComboBoxStyleHelper.h"
#include "scene/controller/objects/dialog/ChangeBodyShapeDialog.h"
#include "scene/controller/objects/bodyshape/BodyShapeWidgetRetriever.h"
#include "scene/controller/objects/bodyshape/support/DefaultBodyShapeCreator.h"

namespace urchin
{

	BodyCompoundShapeWidget::BodyCompoundShapeWidget(QWidget *parent, const SceneObject *sceneObject) :
			BodyShapeWidget(parent, sceneObject)
	{
		shapesLabel = new QLabel(this);
		shapesLabel->setText("Shapes:");
		shapesLabel->setGeometry(QRect(5, 0, 80, 22));

		localizedShapeTableView = new LocalizedShapeTableView(this);
		localizedShapeTableView->addObserver(this, LocalizedShapeTableView::SELECTION_CHANGED);
		localizedShapeTableView->setGeometry(QRect(5, 25, 220, 100));

		addShapeButton = new QPushButton(this);
		addShapeButton->setText("New Shape");
		addShapeButton->setGeometry(QRect(5, 126, 90, 22));
		connect(addShapeButton, SIGNAL(clicked()), this, SLOT(addNewLocalizedShape()));

		removeShapeButton = new QPushButton(this);
		removeShapeButton->setText("Remove Shape");
		removeShapeButton->setGeometry(QRect(96, 126, 90, 22));
		connect(removeShapeButton, SIGNAL(clicked()), this, SLOT(removeSelectedLocalizedShape()));

		localizedShapeDetails = nullptr;
		positionX = nullptr;
		positionY = nullptr;
		positionZ = nullptr;
		orientationType = nullptr;
		eulerAxis0 = nullptr;
		eulerAxis1 = nullptr;
		eulerAxis2 = nullptr;
		bodyShapeWidget = nullptr;
	}

	BodyCompoundShapeWidget::~BodyCompoundShapeWidget()
	{

	}

	std::string BodyCompoundShapeWidget::getBodyShapeName() const
	{
		return COMPOUND_SHAPE_LABEL;
	}

	LocalizedShapeTableView *BodyCompoundShapeWidget::getLocalizedShapeTableView() const
	{
		return localizedShapeTableView;
	}

	void BodyCompoundShapeWidget::doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D> shape)
	{
		const CollisionCompoundShape *compoundShape = static_cast<const CollisionCompoundShape *>(shape.get());

		const std::vector<std::shared_ptr<const LocalizedCollisionShape>> &localizedShapes = compoundShape->getLocalizedShapes();
		for(unsigned int i=0; i<localizedShapes.size(); ++i)
		{
			localizedShapeTableView->addLocalizedShape(localizedShapes[i]);
		}
		localizedShapeTableView->selectLocalizedShape(0);
	}

	std::shared_ptr<const CollisionShape3D> BodyCompoundShapeWidget::createBodyShape() const
	{
		try
		{
			LabelStyleHelper::applyNormalStyle(shapesLabel);
			std::vector<std::shared_ptr<const LocalizedCollisionShape>> localizedCollisionShapes = localizedShapeTableView->getLocalizedShapes();
			return std::make_shared<const CollisionCompoundShape>(localizedCollisionShapes);
		}catch(std::invalid_argument &e)
		{
			LabelStyleHelper::applyErrorStyle(shapesLabel, std::string(e.what()));
			return DefaultBodyShapeCreator(getSceneObject()).createDefaultBodyShape(CollisionShape3D::ShapeType::COMPOUND_SHAPE, true);
		}
	}

	void BodyCompoundShapeWidget::notify(Observable *observable, int notificationType)
	{
		if(LocalizedShapeTableView *localizedShapeTableView = dynamic_cast<LocalizedShapeTableView *>(observable))
		{
			switch(notificationType)
			{
				case LocalizedShapeTableView::SELECTION_CHANGED:
					if(localizedShapeTableView->hasLocalizedShapeSelected())
					{
						std::shared_ptr<const LocalizedCollisionShape> localizedShape = localizedShapeTableView->getSelectedLocalizedShape();

						delete localizedShapeDetails;
						localizedShapeDetails = new QWidget(this);
						localizedShapeDetails->setGeometry(QRect(10, 155, 358, 315));

						setupTransformBox(localizedShapeDetails, localizedShape);
						setupShapeBox(localizedShapeDetails, localizedShape);

						localizedShapeDetails->show();
					}else
					{
						delete localizedShapeDetails;
						localizedShapeDetails = nullptr;
					}
					break;
			}
		}
	}

	void BodyCompoundShapeWidget::setupTransformBox(QWidget *tabGeneral, std::shared_ptr<const LocalizedCollisionShape> localizedShape)
	{
		QGroupBox *transformGroupBox = new QGroupBox(tabGeneral);
		transformGroupBox->setTitle("Transform");
		transformGroupBox->setGeometry(QRect(5, 0, 350, 95));
		GroupBoxStyleHelper::applyNormalStyle(transformGroupBox);

		setupPosition(transformGroupBox, localizedShape->transform.getPosition());
		setupOrientation(transformGroupBox,localizedShape->transform.getOrientation());
	}

	void BodyCompoundShapeWidget::setupPosition(QGroupBox *transformGroupBox, const Point3<float> &position)
	{
		QLabel *positionLabel= new QLabel(transformGroupBox);
		positionLabel->setText("Position:");
		positionLabel->setGeometry(QRect(5, 15, 55, 22));

		positionX = new QDoubleSpinBox(transformGroupBox);
		positionX->setGeometry(QRect(85, 15, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(positionX);
		positionX->setValue(position.X);
		connect(positionX, SIGNAL(valueChanged(double)), this, SLOT(updateSelectedLocalizedShape()));

		positionY = new QDoubleSpinBox(transformGroupBox);
		positionY->setGeometry(QRect(165, 15, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(positionY);
		positionY->setValue(position.Y);
		connect(positionY, SIGNAL(valueChanged(double)), this, SLOT(updateSelectedLocalizedShape()));

		positionZ = new QDoubleSpinBox(transformGroupBox);
		positionZ->setGeometry(QRect(245, 15, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(positionZ);
		positionZ->setValue(position.Z);
		connect(positionZ, SIGNAL(valueChanged(double)), this, SLOT(updateSelectedLocalizedShape()));
	}

	void BodyCompoundShapeWidget::setupOrientation(QGroupBox *transformGroupBox, const Quaternion<float> &orientation)
	{
		QLabel *orientationTypeLabel = new QLabel(transformGroupBox);
		orientationTypeLabel->setText("Orient. Type:");
		orientationTypeLabel->setGeometry(QRect(5, 40, 80, 22));

		orientationType = new QComboBox(transformGroupBox);
		orientationType->setGeometry(QRect(85, 40, 160, 22));
		ComboBoxStyleHelper::applyOrientationStyleOn(orientationType);
		connect(orientationType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateLocalizedShapeOrientationType()));

		QLabel *eulerAngleLabel = new QLabel(transformGroupBox);
		eulerAngleLabel->setText("Euler Angle:");
		eulerAngleLabel->setGeometry(QRect(5, 65, 80, 22));

		QVariant variant = orientationType->currentData();
		Quaternion<float>::RotationSequence newRotationSequence = static_cast<Quaternion<float>::RotationSequence>(variant.toInt());
		Vector3<float> eulerAngle = orientation.toEulerAngle(newRotationSequence);

		eulerAxis0 = new QDoubleSpinBox(transformGroupBox);
		eulerAxis0->setGeometry(QRect(85, 65, 80, 22));
		SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis0);
		eulerAxis0->setValue(AngleConverter<float>::toDegree(eulerAngle.X));
		connect(eulerAxis0, SIGNAL(valueChanged(double)), this, SLOT(updateSelectedLocalizedShape()));

		eulerAxis1 = new QDoubleSpinBox(transformGroupBox);
		eulerAxis1->setGeometry(QRect(165, 65, 80, 22));
		SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis1);
		eulerAxis1->setValue(AngleConverter<float>::toDegree(eulerAngle.Y));
		connect(eulerAxis1, SIGNAL(valueChanged(double)), this, SLOT(updateSelectedLocalizedShape()));

		eulerAxis2 = new QDoubleSpinBox(transformGroupBox);
		eulerAxis2->setGeometry(QRect(245, 65, 80, 22));
		SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis2);
		eulerAxis2->setValue(AngleConverter<float>::toDegree(eulerAngle.Z));
		connect(eulerAxis2, SIGNAL(valueChanged(double)), this, SLOT(updateSelectedLocalizedShape()));
	}

	void BodyCompoundShapeWidget::setupShapeBox(QWidget *tabGeneral, std::shared_ptr<const LocalizedCollisionShape> localizedShape)
	{
		QGroupBox *shapeGroupBox = new QGroupBox(tabGeneral);
		shapeGroupBox->setTitle("Shape");
		shapeGroupBox->setGeometry(QRect(5, 105, 350, 210));
		GroupBoxStyleHelper::applyNormalStyle(shapeGroupBox);

		BodyShapeWidgetRetriever shapeWidgetRetriever(shapeGroupBox, getSceneObject());
		bodyShapeWidget = shapeWidgetRetriever.retrieveShapeWidget(localizedShape->shape->getShapeType());
		bodyShapeWidget->setGeometry(QRect(5, 15, 338, 165));
		bodyShapeWidget->setupShapePropertiesFrom(localizedShape->shape);
		connect(bodyShapeWidget, SIGNAL(bodyShapeChange(std::shared_ptr<const CollisionShape3D>)), this, SLOT(updateSelectedLocalizedShape()));
	}

	void BodyCompoundShapeWidget::updateLocalizedShapeOrientationType()
	{
		if(!disableShapeEvent)
		{
			std::shared_ptr<const LocalizedCollisionShape> localizedShape = localizedShapeTableView->getSelectedLocalizedShape();

			QVariant variant = orientationType->currentData();
			Quaternion<float>::RotationSequence newRotationSequence = static_cast<Quaternion<float>::RotationSequence>(variant.toInt());
			Vector3<float> eulerAngle = localizedShape->transform.getOrientation().toEulerAngle(newRotationSequence);

			eulerAxis0->setValue(AngleConverter<float>::toDegree(eulerAngle.X));
			eulerAxis1->setValue(AngleConverter<float>::toDegree(eulerAngle.Y));
			eulerAxis2->setValue(AngleConverter<float>::toDegree(eulerAngle.Z));

			updateSelectedLocalizedShape();
		}
	}

	void BodyCompoundShapeWidget::updateSelectedLocalizedShape()
	{
		if(!disableShapeEvent)
		{
			if(!localizedShapeTableView->hasLocalizedShapeSelected())
			{
				throw std::invalid_argument("Localized shape table hasn't localized shaped selected");
			}

			std::shared_ptr<LocalizedCollisionShape> localizedShape = std::make_shared<LocalizedCollisionShape>();

			localizedShape->position = localizedShapeTableView->getSelectedLocalizedShape()->position;
			localizedShape->shape = bodyShapeWidget->retrieveShape();

			Vector3<float> eulerAngle(
					AngleConverter<float>::toRadian(eulerAxis0->value()),
					AngleConverter<float>::toRadian(eulerAxis1->value()),
					AngleConverter<float>::toRadian(eulerAxis2->value())
			);
			QVariant variant = orientationType->currentData();
			Quaternion<float>::RotationSequence rotationSequence = static_cast<Quaternion<float>::RotationSequence>(variant.toInt());
			localizedShape->transform = PhysicsTransform(
					Point3<float>(positionX->value(), positionY->value(), positionZ->value()),
					Quaternion<float>(eulerAngle, rotationSequence));

			localizedShapeTableView->updateSelectedLocalizedShape(localizedShape);

			updateBodyShape();
		}
	}

	void BodyCompoundShapeWidget::addNewLocalizedShape()
	{
		ChangeBodyShapeDialog changeBodyShapeDialog(this, true);
		changeBodyShapeDialog.exec();

		if(changeBodyShapeDialog.result()==QDialog::Accepted)
		{
			CollisionShape3D::ShapeType shapeType = changeBodyShapeDialog.getShapeType();
			std::shared_ptr<const CollisionShape3D> defaultNewShape = DefaultBodyShapeCreator(getSceneObject()).createDefaultBodyShape(shapeType, true);

			unsigned int nextPosition = 0;
			for(unsigned int i=0; i<localizedShapeTableView->getLocalizedShapes().size(); ++i)
			{
				if(nextPosition<=localizedShapeTableView->getLocalizedShapes()[i]->position)
				{
					nextPosition = localizedShapeTableView->getLocalizedShapes()[i]->position + 1;
				}
			}

			std::shared_ptr<LocalizedCollisionShape> localizedShape = std::make_shared<LocalizedCollisionShape>();
			localizedShape->position = nextPosition;
			localizedShape->shape = defaultNewShape;
			localizedShape->transform = PhysicsTransform();

			int rowId = localizedShapeTableView->addLocalizedShape(localizedShape);
			localizedShapeTableView->selectLocalizedShape(rowId);

			updateBodyShape();
		}
	}

	void BodyCompoundShapeWidget::removeSelectedLocalizedShape()
	{
		if(localizedShapeTableView->removeSelectedLocalizedShape())
		{
			updateBodyShape();
		}
	}
}
