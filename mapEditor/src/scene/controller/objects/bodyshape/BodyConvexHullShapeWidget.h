#ifndef ENGINE_MAPEDITOR_BODYCONVEXHULLSHAPEWIDGET_H
#define ENGINE_MAPEDITOR_BODYCONVEXHULLSHAPEWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QTableView>
#include <QtWidgets/QPushButton>
#include <QStandardItemModel>

#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"
#include "scene/controller/objects/bodyshape/BodyShapeWidget.h"

namespace urchin
{

	class BodyConvexHullShapeWidget : public BodyShapeWidget
	{
		Q_OBJECT

		public:
			BodyConvexHullShapeWidget(const SceneObject *);
			virtual ~BodyConvexHullShapeWidget();

			std::string getBodyShapeName() const;

		protected:
			void doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D>);
			std::shared_ptr<const CollisionShape3D> createBodyShape() const;

		private:
			void addPoint(const Point3<float> &);
			std::vector<Point3<float>> getPoints() const;

			QLabel *pointsLabel;
			QTableView *pointsTable;
			QStandardItemModel *pointsTableModel;
			QPushButton *addPointButton, *removePointButton;

		private slots:
			void addNewPoint();
			void removeSelectedPoint();

	};

}

#endif
