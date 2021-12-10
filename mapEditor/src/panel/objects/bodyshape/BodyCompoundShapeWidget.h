#pragma once

#include <string>
#include <memory>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGridLayout>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include <panel/objects/bodyshape/BodyShapeWidget.h>
#include <panel/objects/bodyshape/support/LocalizedShapeTableView.h>

namespace urchin {

    class BodyCompoundShapeWidget : public BodyShapeWidget, public Observer {
        Q_OBJECT

        public:
            explicit BodyCompoundShapeWidget(const ObjectEntity*);
            ~BodyCompoundShapeWidget() override = default;

            std::string getBodyShapeName() const override;
            LocalizedShapeTableView* getLocalizedShapeTableView() const;

        protected:
            void doSetupShapePropertiesFrom(const CollisionShape3D&) override;
            std::unique_ptr<const CollisionShape3D> createBodyShape() const override;

        private:
            void notify(Observable*, int) override;

            void setupTransformBox(QVBoxLayout*, const LocalizedCollisionShape*);
            void setupPosition(QGridLayout*, const Point3<float>&);
            void setupOrientation(QGridLayout*, const Quaternion<float>&);
            void setupShapeBox(QVBoxLayout*, const LocalizedCollisionShape*);

            QLabel* shapesLabel;
            LocalizedShapeTableView* localizedShapeTableView;
            QPushButton* addShapeButton;
            QPushButton* removeShapeButton;

            std::unique_ptr<QWidget> localizedShapeDetails;
            QDoubleSpinBox* positionX;
            QDoubleSpinBox* positionY;
            QDoubleSpinBox* positionZ;
            QComboBox* orientationType;
            QDoubleSpinBox* eulerAxis0;
            QDoubleSpinBox* eulerAxis1;
            QDoubleSpinBox* eulerAxis2;
            BodyShapeWidget* bodyShapeWidget;

        private slots:
            void addNewLocalizedShape();
            void removeSelectedLocalizedShape();
            void updateLocalizedShapeOrientationType();
            void updateSelectedLocalizedShape();

    };

}
