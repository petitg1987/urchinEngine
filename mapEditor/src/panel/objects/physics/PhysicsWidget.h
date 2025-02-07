#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QPushButton>
#include <UrchinPhysicsEngine.h>

#include <controller/objects/ObjectController.h>
#include <panel/objects/physics/bodyshape/BodyShapeWidget.h>

namespace urchin {

    class PhysicsWidget final : public QWidget, public Observable {
        Q_OBJECT

        public:
            PhysicsWidget();

            enum NotificationType {
                OBJECT_BODY_SHAPE_WIDGET_CREATED
            };

            void load(const ObjectEntity&, ObjectController&);

            BodyShapeWidget* getBodyShapeWidget() const;

        private:
            void setupPhysicsGeneralPropertiesBox(QVBoxLayout*);
            void setupPhysicsDampingPropertiesBox(QVBoxLayout*);
            void setupPhysicsFactorPropertiesBox(QVBoxLayout*);
            void setupPhysicsShapeBox(QVBoxLayout*);

            void setupObjectPhysicsDataFrom(const RigidBody*);
            BodyShapeWidget& createBodyShapeWidget(const CollisionShape3D&, const ObjectEntity&);
            BodyShapeWidget& createNoBodyShapeWidget(const ObjectEntity&);
            void setupBodyShapeWidget();

            const ObjectEntity* objectEntity;
            ObjectController* objectController;

            bool disablePhysicsEvent;

            QTabWidget* tabPhysicsRigidBody;
            QVBoxLayout* physicsShapeLayout;
            QDoubleSpinBox* mass;
            QDoubleSpinBox* restitution;
            QDoubleSpinBox* friction;
            QDoubleSpinBox* rollingFriction;
            QDoubleSpinBox* linearDamping;
            QDoubleSpinBox* angularDamping;
            QDoubleSpinBox* linearFactorX;
            QDoubleSpinBox* linearFactorY;
            QDoubleSpinBox* linearFactorZ;
            QDoubleSpinBox* angularFactorX;
            QDoubleSpinBox* angularFactorY;
            QDoubleSpinBox* angularFactorZ;
            QLabel* shapeTypeValueLabel;
            QPushButton* changeBodyShapeButton;
            BodyShapeWidget* bodyShapeWidget;

        private slots:
            void showChangeBodyShapeDialog();
            void updateObjectPhysicsProperties() const;
            void bodyShapeChanged(std::unique_ptr<const CollisionShape3D>&) const;
    };

}
