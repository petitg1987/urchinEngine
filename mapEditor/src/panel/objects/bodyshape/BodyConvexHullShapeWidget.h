#pragma once

#include <string>
#include <memory>
#include <QtWidgets/QTableView>
#include <QtWidgets/QPushButton>
#include <QStandardItemModel>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include <panel/objects/bodyshape/BodyShapeWidget.h>

namespace urchin {

    class BodyConvexHullShapeWidget final : public BodyShapeWidget {
        Q_OBJECT

        public:
            explicit BodyConvexHullShapeWidget(const ObjectEntity*);
            ~BodyConvexHullShapeWidget() override = default;

            std::string getBodyShapeName() const override;

        protected:
            void doSetupShapePropertiesFrom(const CollisionShape3D&) override;
            std::unique_ptr<const CollisionShape3D> createBodyShape() const override;

        private:
            void addPoint(const Point3<float>&) const;
            std::vector<Point3<float>> getPoints() const;

            QLabel* pointsLabel;
            QTableView* pointsTable;
            QStandardItemModel* pointsTableModel;
            QPushButton* addPointButton;
            QPushButton* removePointButton;

        private slots:
            void addNewPoint();
            void removeSelectedPoint();

    };

}
