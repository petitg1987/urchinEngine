#include <QtWidgets/QLabel>
#include <QVariant>
#include <QHeaderView>

#include "BodyConvexHullShapeWidget.h"
#include "widget/style/LabelStyleHelper.h"
#include "widget/style/ButtonStyleHelper.h"
#include "panel/objects/bodyshape/support/SpinBoxDelegate.h"
#include "panel/objects/bodyshape/support/DefaultBodyShapeCreator.h"

namespace urchin {

    BodyConvexHullShapeWidget::BodyConvexHullShapeWidget(const SceneObject* sceneObject) :
            BodyShapeWidget(sceneObject) {
        pointsLabel = new QLabel("Points:");
        mainLayout->addWidget(pointsLabel, 0, 0);

        pointsTableModel = new QStandardItemModel(0, 3);
        pointsTableModel->setHorizontalHeaderItem(0, new QStandardItem("X"));
        pointsTableModel->setHorizontalHeaderItem(1, new QStandardItem("Y"));
        pointsTableModel->setHorizontalHeaderItem(2, new QStandardItem("Z"));
        connect(pointsTableModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(updateBodyShape()));

        pointsTable = new QTableView();
        mainLayout->addWidget(pointsTable, 1, 0);
        pointsTable->setModel(pointsTableModel);
        pointsTable->setFixedHeight(185);
        pointsTable->horizontalHeader()->resizeSection(0, 90);
        pointsTable->horizontalHeader()->resizeSection(1, 90);
        pointsTable->horizontalHeader()->resizeSection(2, 90);

        pointsTable->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        pointsTable->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

        auto* delegate = new SpinBoxDelegate(nullptr, this, pointsTable, pointsTableModel);
        pointsTable->setItemDelegate(delegate);

        auto* buttonLayout = new QHBoxLayout();
        mainLayout->addLayout(buttonLayout, 2, 0);
        buttonLayout->setAlignment(Qt::AlignLeft);

        addPointButton = new QPushButton("New Point");
        buttonLayout->addWidget(addPointButton);
        ButtonStyleHelper::applyNormalStyle(addPointButton);
        connect(addPointButton, SIGNAL(clicked()), this, SLOT(addNewPoint()));

        removePointButton = new QPushButton("Remove Point");
        buttonLayout->addWidget(removePointButton);
        ButtonStyleHelper::applyNormalStyle(removePointButton);
        connect(removePointButton, SIGNAL(clicked()), this, SLOT(removeSelectedPoint()));
    }

    std::string BodyConvexHullShapeWidget::getBodyShapeName() const {
        return CONVEX_HULL_SHAPE_LABEL;
    }

    void BodyConvexHullShapeWidget::doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D> shape) {
        const auto* convexHullShape = dynamic_cast<const CollisionConvexHullShape *>(shape.get());

        const std::vector<Point3<float>>& points = convexHullShape->getPoints();
        for (const auto& point : points) {
            addPoint(point);
        }
    }

    std::shared_ptr<const CollisionShape3D> BodyConvexHullShapeWidget::createBodyShape() const {
        try {
            LabelStyleHelper::applyNormalStyle(pointsLabel);
            auto scaledShape = std::make_shared<const CollisionConvexHullShape>(getPoints());

            //test construction of original shape because can throw an exception due to imprecision of float
            float invScale = 1.0f / getSceneObject()->getModel()->getTransform().getScale();
            scaledShape->scale(invScale);

            return scaledShape;
        } catch (std::invalid_argument& e) {
            LabelStyleHelper::applyErrorStyle(pointsLabel, std::string(e.what()));
            return DefaultBodyShapeCreator(getSceneObject()).createDefaultBodyShape(CollisionShape3D::ShapeType::CONVEX_HULL_SHAPE, true);
        }
    }

    std::vector<Point3<float>> BodyConvexHullShapeWidget::getPoints() const {
        std::vector<Point3<float>> points;
        for (int row=0; row< pointsTableModel->rowCount(); ++row) {
            QModelIndex indexX = pointsTableModel->index(row, 0);
            QModelIndex indexY = pointsTableModel->index(row, 1);
            QModelIndex indexZ = pointsTableModel->index(row, 2);

            QVariant valueX = pointsTableModel->data(indexX);
            QVariant valueY = pointsTableModel->data(indexY);
            QVariant valueZ = pointsTableModel->data(indexZ);

            points.emplace_back(Point3<float>(valueX.toFloat(), valueY.toFloat(), valueZ.toFloat()));
        }

        return points;
    }

    void BodyConvexHullShapeWidget::addPoint(const Point3<float>& point) {
        pointsTableModel->insertRow(pointsTableModel->rowCount());

        QModelIndex indexX = pointsTableModel->index(pointsTableModel->rowCount()-1, 0);
        pointsTableModel->setData(indexX, QVariant(point.X));

        QModelIndex indexY = pointsTableModel->index(pointsTableModel->rowCount()-1, 1);
        pointsTableModel->setData(indexY, QVariant(point.Y));

        QModelIndex indexZ = pointsTableModel->index(pointsTableModel->rowCount()-1, 2);
        pointsTableModel->setData(indexZ, QVariant(point.Z));

        pointsTable->resizeRowsToContents();
    }

    void BodyConvexHullShapeWidget::addNewPoint() {
        addPoint(Point3<float>(0.0, 0.0, 0.0));

        updateBodyShape();
    }

    void BodyConvexHullShapeWidget::removeSelectedPoint() {
        if (pointsTable->currentIndex().row()!=-1) {
            pointsTableModel->removeRow(pointsTable->currentIndex().row());
            pointsTable->resizeRowsToContents();

            updateBodyShape();
        }
    }
}
