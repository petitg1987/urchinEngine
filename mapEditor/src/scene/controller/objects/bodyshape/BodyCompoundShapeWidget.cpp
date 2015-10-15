#include <stdexcept>

#include "BodyCompoundShapeWidget.h"
#include "support/LabelStyleHelper.h"
#include "support/SpinBoxStyleHelper.h"
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
		addShapeButton->setGeometry(QRect(5, 126, 85, 22));
		connect(addShapeButton, SIGNAL(clicked()), this, SLOT(addNewLocalizedShape()));

		removeShapeButton = new QPushButton(this);
		removeShapeButton->setText("Remove Shape");
		removeShapeButton->setGeometry(QRect(91, 126, 85, 22));
		connect(removeShapeButton, SIGNAL(clicked()), this, SLOT(removeSelectedLocalizedShape()));

		localizedShapeDetails = nullptr;
		translationX = nullptr;
		translationY = nullptr;
		translationZ = nullptr;
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
						localizedShapeDetails->setGeometry(QRect(10, 155, 338, 220));

						QLabel *translationLabel = new QLabel(localizedShapeDetails);
						translationLabel->setText("Translation:");
						translationLabel->setGeometry(QRect(5, 0, 80, 22));

						translationX = new QDoubleSpinBox(localizedShapeDetails);
						translationX->setGeometry(QRect(85, 0, 80, 22));
						SpinBoxStyleHelper::applyNormalStyle(translationX);
						translationX->setValue(localizedShape->translation.X);

						translationY = new QDoubleSpinBox(localizedShapeDetails);
						translationY->setGeometry(QRect(165, 0, 80, 22));
						SpinBoxStyleHelper::applyNormalStyle(translationY);
						translationY->setValue(localizedShape->translation.Y);

						translationZ = new QDoubleSpinBox(localizedShapeDetails);
						translationZ->setGeometry(QRect(245, 0, 80, 22));
						SpinBoxStyleHelper::applyNormalStyle(translationZ);
						translationZ->setValue(localizedShape->translation.Z);

						BodyShapeWidgetRetriever shapeWidgetRetriever(localizedShapeDetails, getSceneObject());
						bodyShapeWidget = shapeWidgetRetriever.retrieveShapeWidget(localizedShape->shape->getShapeType());
						bodyShapeWidget->setGeometry(QRect(0, 25, 338, 165));
						bodyShapeWidget->setupShapePropertiesFrom(localizedShape->shape);

						connect(translationX, SIGNAL(valueChanged(double)), this, SLOT(updateSelectedLocalizedShape()));
						connect(translationY, SIGNAL(valueChanged(double)), this, SLOT(updateSelectedLocalizedShape()));
						connect(translationZ, SIGNAL(valueChanged(double)), this, SLOT(updateSelectedLocalizedShape()));
						connect(bodyShapeWidget, SIGNAL(bodyShapeChange(std::shared_ptr<const CollisionShape3D>)), this, SLOT(updateSelectedLocalizedShape()));

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

	void BodyCompoundShapeWidget::updateSelectedLocalizedShape()
	{
		if(!localizedShapeTableView->hasLocalizedShapeSelected())
		{
			throw std::invalid_argument("Localized shape table hasn't localized shaped selected");
		}

		std::shared_ptr<LocalizedCollisionShape> localizedShape = std::make_shared<LocalizedCollisionShape>();
		localizedShape->position = localizedShapeTableView->getSelectedLocalizedShape()->position;
		localizedShape->shape = bodyShapeWidget->retrieveShape();
		localizedShape->translation = Vector3<float>(translationX->value(), translationY->value(), translationZ->value());
		localizedShapeTableView->updateSelectedLocalizedShape(localizedShape);

		updateBodyShape();
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
			localizedShape->translation = Vector3<float>(0.0, 0.0, 0.0);

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
