#include <QtWidgets/QLabel>
#include <QHeaderView>
#include <QVariant>

#include "BodyConvexHullShapeWidget.h"
#include "support/SpinBoxStyleHelper.h"
#include "support/LabelStyleHelper.h"
#include "scene/controller/objects/bodyshape/support/SpinBoxDelegate.h"
#include "scene/controller/objects/bodyshape/support/DefaultBodyShapeCreator.h"

namespace urchin
{

	BodyConvexHullShapeWidget::BodyConvexHullShapeWidget(QWidget *parent, const SceneObject *sceneObject) :
			BodyShapeWidget(parent, sceneObject)
	{
		pointsLabel = new QLabel(this);
		pointsLabel->setText("Points:");
		pointsLabel->setGeometry(QRect(5, 0, 80, 22));

		pointsTableModel = new QStandardItemModel(0, 3, this);
		pointsTableModel->setHorizontalHeaderItem(0, new QStandardItem("X"));
		pointsTableModel->setHorizontalHeaderItem(1, new QStandardItem("Y"));
		pointsTableModel->setHorizontalHeaderItem(2, new QStandardItem("Z"));
		connect(pointsTableModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(updateBodyShape()));

		pointsTable = new QTableView(this);
		pointsTable->setModel(pointsTableModel);
		pointsTable->setGeometry(QRect(5, 25, 310, 110));
		pointsTable->horizontalHeader()->resizeSection(0, 90);
		pointsTable->horizontalHeader()->resizeSection(1, 90);
		pointsTable->horizontalHeader()->resizeSection(2, 90);

		pointsTable->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
		pointsTable->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

		SpinBoxDelegate *delegate = new SpinBoxDelegate(NULL, this, pointsTable, pointsTableModel);
		pointsTable->setItemDelegate(delegate);

		addPointButton = new QPushButton(this);
		addPointButton->setText("New Point");
		addPointButton->setGeometry(QRect(5, 136, 85, 22));
		connect(addPointButton, SIGNAL(clicked()), this, SLOT(addNewPoint()));

		removePointButton = new QPushButton(this);
		removePointButton->setText("Remove Point");
		removePointButton->setGeometry(QRect(91, 136, 85, 22));
		connect(removePointButton, SIGNAL(clicked()), this, SLOT(removeSelectedPoint()));
	}

	BodyConvexHullShapeWidget::~BodyConvexHullShapeWidget()
	{

	}

	std::string BodyConvexHullShapeWidget::getBodyShapeName() const
	{
		return CONVEX_HULL_SHAPE_LABEL;
	}

	void BodyConvexHullShapeWidget::doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D> shape)
	{
		const CollisionConvexHullShape *convexHullShape = static_cast<const CollisionConvexHullShape *>(shape.get());

		const std::vector<Point3<float>> &points = convexHullShape->getPoints();
		for(unsigned int i=0; i<points.size(); ++i)
		{
			addPoint(points[i]);
		}
	}

	std::shared_ptr<const CollisionShape3D> BodyConvexHullShapeWidget::createBodyShape() const
	{
		try
		{
			LabelStyleHelper::applyNormalStyle(pointsLabel);
			auto scaledShape = std::make_shared<const CollisionConvexHullShape>(getPoints());

			//construction of original shape throw an exception due to imprecision of float?
			float invScale = 1.0 / getSceneObject()->getModel()->getTransform().getScale();
			scaledShape->scale(invScale);

			return scaledShape;
		}catch(std::invalid_argument &e)
		{
			LabelStyleHelper::applyErrorStyle(pointsLabel, std::string(e.what()));
			return DefaultBodyShapeCreator(getSceneObject()).createDefaultBodyShape(CollisionShape3D::ShapeType::CONVEX_HULL_SHAPE, true);
		}
	}

	std::vector<Point3<float>> BodyConvexHullShapeWidget::getPoints() const
	{
		std::vector<Point3<float>> points;
		for(int row=0; row< pointsTableModel->rowCount(); ++row)
		{
			QModelIndex indexX = pointsTableModel->index(row, 0);
			QModelIndex indexY = pointsTableModel->index(row, 1);
			QModelIndex indexZ = pointsTableModel->index(row, 2);

			QVariant valueX = pointsTableModel->data(indexX);
			QVariant valueY = pointsTableModel->data(indexY);
			QVariant valueZ = pointsTableModel->data(indexZ);

			points.push_back(Point3<float>(valueX.toFloat(), valueY.toFloat(), valueZ.toFloat()));
		}

		return points;
	}

	void BodyConvexHullShapeWidget::addPoint(const Point3<float> &point)
	{
		pointsTableModel->insertRow(pointsTableModel->rowCount());

		QModelIndex indexX = pointsTableModel->index(pointsTableModel->rowCount()-1, 0);
		pointsTableModel->setData(indexX, QVariant(point.X));

		QModelIndex indexY = pointsTableModel->index(pointsTableModel->rowCount()-1, 1);
		pointsTableModel->setData(indexY, QVariant(point.Y));

		QModelIndex indexZ = pointsTableModel->index(pointsTableModel->rowCount()-1, 2);
		pointsTableModel->setData(indexZ, QVariant(point.Z));

		pointsTable->resizeRowsToContents();
	}

	void BodyConvexHullShapeWidget::addNewPoint()
	{
		addPoint(Point3<float>(0.0, 0.0, 0.0));

		updateBodyShape();
	}

	void BodyConvexHullShapeWidget::removeSelectedPoint()
	{
		if(pointsTable->currentIndex().row()!=-1)
		{
			pointsTableModel->removeRow(pointsTable->currentIndex().row());
			pointsTable->resizeRowsToContents();

			updateBodyShape();
		}
	}
}
