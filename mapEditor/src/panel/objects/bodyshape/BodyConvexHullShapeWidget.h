#ifndef URCHINENGINE_BODYCONVEXHULLSHAPEWIDGET_H
#define URCHINENGINE_BODYCONVEXHULLSHAPEWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QTableView>
#include <QtWidgets/QPushButton>
#include <QStandardItemModel>

#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"
#include "panel/objects/bodyshape/BodyShapeWidget.h"

namespace urchin
{

    class BodyConvexHullShapeWidget : public BodyShapeWidget
    {
        Q_OBJECT

        public:
            explicit BodyConvexHullShapeWidget(const SceneObject *);
            ~BodyConvexHullShapeWidget() override = default;

            std::string getBodyShapeName() const override;

        protected:
            void doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D>) override;
            std::shared_ptr<const CollisionShape3D> createBodyShape() const override;

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
