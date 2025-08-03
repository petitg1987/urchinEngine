#include "panel/object/physics/PhysicsWidget.h"
#include "panel/object/physics/dialog/ChangeBodyShapeDialog.h"
#include "panel/object/physics/bodyshape/NoBodyShapeWidget.h"
#include "panel/object/physics/bodyshape/BodyShapeWidgetRetriever.h"
#include "widget/style/FrameStyleHelper.h"
#include "widget/style/GroupBoxStyleHelper.h"
#include "widget/style/SpinBoxStyleHelper.h"
#include "widget/style/ButtonStyleHelper.h"

namespace urchin {

    PhysicsWidget::PhysicsWidget() :
            objectEntity(nullptr),
            objectController(nullptr),
            disablePhysicsEvent(false),
            tabPhysicsRigidBody(nullptr),
            physicsShapeLayout(nullptr),
            mass(nullptr),
            restitution(nullptr),
            friction(nullptr),
            rollingFriction(nullptr),
            linearDamping(nullptr),
            angularDamping(nullptr),
            linearFactorX(nullptr),
            linearFactorY(nullptr),
            linearFactorZ(nullptr),
            angularFactorX(nullptr),
            angularFactorY(nullptr),
            angularFactorZ(nullptr),
            shapeTypeValueLabel(nullptr),
            bodyShapeWidget(nullptr) {
        setContentsMargins(0, 0, 0, 0);

        auto* mainLayout = new QVBoxLayout(this);
        mainLayout->setAlignment(Qt::AlignTop);
        mainLayout->setContentsMargins(0, 0, 0, 0);

        auto* tabPhysicsRigidBody = new QTabWidget(this);
        mainLayout->addWidget(tabPhysicsRigidBody);

        auto* tabPhysicsProperties = new QWidget();
        auto* physicsPropertiesLayout = new QVBoxLayout(tabPhysicsProperties);
        physicsPropertiesLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
        physicsPropertiesLayout->setContentsMargins(1, 1, 1, 1);
        setupPhysicsGeneralPropertiesBox(physicsPropertiesLayout);
        setupPhysicsDampingPropertiesBox(physicsPropertiesLayout);
        setupPhysicsFactorPropertiesBox(physicsPropertiesLayout);
        tabPhysicsRigidBody->addTab(tabPhysicsProperties, "Properties");

        auto* tabPhysicsShape = new QWidget();
        physicsShapeLayout = new QVBoxLayout(tabPhysicsShape);
        physicsShapeLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
        physicsShapeLayout->setContentsMargins(1, 1, 1, 1);
        setupPhysicsShapeBox(physicsShapeLayout);
        tabPhysicsRigidBody->addTab(tabPhysicsShape, "Shape");
    }

    void PhysicsWidget::load(const ObjectEntity& objectEntity, ObjectController& objectController) {
        this->objectEntity = &objectEntity;
        this->objectController = &objectController;

        setupObjectPhysicsDataFrom(objectEntity.getRigidBody());
    }

    BodyShapeWidget* PhysicsWidget::getBodyShapeWidget() const {
        return bodyShapeWidget;
    }

    void PhysicsWidget::setupPhysicsGeneralPropertiesBox(QVBoxLayout* physicsPropertiesLayout) {
        auto* rigidBodyGeneralBox = new QGroupBox("General");
        physicsPropertiesLayout->addWidget(rigidBodyGeneralBox);
        GroupBoxStyleHelper::applyDefaultStyle(rigidBodyGeneralBox);

        auto* rigidBodyGeneralLayout = new QGridLayout(rigidBodyGeneralBox);

        auto* massLabel = new QLabel("Mass:");
        rigidBodyGeneralLayout->addWidget(massLabel, 0, 0);

        mass = new QDoubleSpinBox();
        rigidBodyGeneralLayout->addWidget(mass, 0, 1);
        SpinBoxStyleHelper::applyDefaultStyle(mass, 0.0);
        connect(mass, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));

        auto* restitutionLabel = new QLabel("Restitution:");
        rigidBodyGeneralLayout->addWidget(restitutionLabel, 0, 2);

        restitution = new QDoubleSpinBox();
        rigidBodyGeneralLayout->addWidget(restitution, 0, 3);
        SpinBoxStyleHelper::applyDefaultStyle(restitution, 0.0, 1.0);
        connect(restitution, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));

        auto* frictionLabel = new QLabel("Friction:");
        rigidBodyGeneralLayout->addWidget(frictionLabel, 1, 0);

        friction = new QDoubleSpinBox();
        rigidBodyGeneralLayout->addWidget(friction, 1, 1);
        SpinBoxStyleHelper::applyDefaultStyle(friction, 0.0, 1.0);
        connect(friction, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));

        auto* rollingFrictionLabel = new QLabel("Rolling Friction:");
        rigidBodyGeneralLayout->addWidget(rollingFrictionLabel, 1, 2);

        rollingFriction = new QDoubleSpinBox();
        rigidBodyGeneralLayout->addWidget(rollingFriction, 1, 3);
        SpinBoxStyleHelper::applyDefaultStyle(rollingFriction, 0.0, 1.0);
        connect(rollingFriction, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));
    }

    void PhysicsWidget::setupPhysicsDampingPropertiesBox(QVBoxLayout* physicsPropertiesLayout) {
        auto* rigidBodyDampingBox = new QGroupBox("Damping");
        physicsPropertiesLayout->addWidget(rigidBodyDampingBox);
        GroupBoxStyleHelper::applyDefaultStyle(rigidBodyDampingBox);

        auto* rigidBodyDampingLayout = new QGridLayout(rigidBodyDampingBox);

        auto* linearDampingLabel = new QLabel("Linear:");
        rigidBodyDampingLayout->addWidget(linearDampingLabel, 0, 0);

        linearDamping = new QDoubleSpinBox();
        rigidBodyDampingLayout->addWidget(linearDamping, 0, 1);
        SpinBoxStyleHelper::applyDefaultStyle(linearDamping, 0.0, 1.0);
        connect(linearDamping, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));

        auto* angularDampingLabel = new QLabel("Angular:");
        rigidBodyDampingLayout->addWidget(angularDampingLabel, 0, 2);

        angularDamping = new QDoubleSpinBox();
        rigidBodyDampingLayout->addWidget(angularDamping, 0, 3);
        SpinBoxStyleHelper::applyDefaultStyle(angularDamping, 0.0, 1.0);
        connect(angularDamping, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));
    }

    void PhysicsWidget::setupPhysicsFactorPropertiesBox(QVBoxLayout* physicsPropertiesLayout) {
        auto* rigidBodyFactorBox = new QGroupBox("Factor");
        physicsPropertiesLayout->addWidget(rigidBodyFactorBox);
        GroupBoxStyleHelper::applyDefaultStyle(rigidBodyFactorBox);

        auto* rigidBodyFactorLayout = new QGridLayout(rigidBodyFactorBox);

        auto* linearFactorLabel = new QLabel("Linear:");
        rigidBodyFactorLayout->addWidget(linearFactorLabel, 0, 0);

        auto* linearFactorLayout = new QHBoxLayout();
        rigidBodyFactorLayout->addLayout(linearFactorLayout, 0, 1);
        linearFactorX = new QDoubleSpinBox();
        linearFactorLayout->addWidget(linearFactorX);
        SpinBoxStyleHelper::applyDefaultStyle(linearFactorX, 0.0, 1.0);
        connect(linearFactorX, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));
        linearFactorY = new QDoubleSpinBox();
        linearFactorLayout->addWidget(linearFactorY);
        SpinBoxStyleHelper::applyDefaultStyle(linearFactorY, 0.0, 1.0);
        connect(linearFactorY, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));
        linearFactorZ = new QDoubleSpinBox();
        linearFactorLayout->addWidget(linearFactorZ);
        SpinBoxStyleHelper::applyDefaultStyle(linearFactorZ, 0.0, 1.0);
        connect(linearFactorZ, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));

        auto* angularFactorLabel = new QLabel("Angular:");
        rigidBodyFactorLayout->addWidget(angularFactorLabel, 1, 0);

        auto* angularFactorLayout = new QHBoxLayout();
        rigidBodyFactorLayout->addLayout(angularFactorLayout, 1, 1);
        angularFactorX = new QDoubleSpinBox();
        angularFactorLayout->addWidget(angularFactorX);
        SpinBoxStyleHelper::applyDefaultStyle(angularFactorX);
        angularFactorX->setMinimum(0.0);
        angularFactorX->setMaximum(1.0);
        connect(angularFactorX, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));
        angularFactorY = new QDoubleSpinBox();
        angularFactorLayout->addWidget(angularFactorY);
        SpinBoxStyleHelper::applyDefaultStyle(angularFactorY);
        angularFactorY->setMinimum(0.0);
        angularFactorY->setMaximum(1.0);
        connect(angularFactorY, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));
        angularFactorZ = new QDoubleSpinBox();
        angularFactorLayout->addWidget(angularFactorZ);
        SpinBoxStyleHelper::applyDefaultStyle(angularFactorZ);
        angularFactorZ->setMinimum(0.0);
        angularFactorZ->setMaximum(1.0);
        connect(angularFactorZ, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));
    }

    void PhysicsWidget::setupPhysicsShapeBox(QVBoxLayout* physicsShapeLayout) {
        auto* shapeTypeLayout = new QHBoxLayout();
        shapeTypeLayout->setAlignment(Qt::AlignLeft);
        shapeTypeLayout->setSpacing(15);
        physicsShapeLayout->addLayout(shapeTypeLayout);

        auto* shapeTypeLabel = new QLabel("Shape Type:");
        shapeTypeLayout->addWidget(shapeTypeLabel);

        shapeTypeValueLabel = new QLabel();
        shapeTypeLayout->addWidget(shapeTypeValueLabel);

        auto* changeBodyShapeButton = new QPushButton("Change");
        shapeTypeLayout->addWidget(changeBodyShapeButton);
        ButtonStyleHelper::applyDefaultStyle(changeBodyShapeButton);
        connect(changeBodyShapeButton, SIGNAL(clicked()), this, SLOT(showChangeBodyShapeDialog()));

        auto* frameLine = new QFrame();
        physicsShapeLayout->addWidget(frameLine);
        FrameStyleHelper::applyLineStyle(frameLine);

        bodyShapeWidget = nullptr;
    }

    void PhysicsWidget::setupObjectPhysicsDataFrom(const RigidBody* rigidBody) {
        disablePhysicsEvent = true;

        if (rigidBody) {
            mass->setValue(rigidBody->getMass());
            restitution->setValue(rigidBody->getRestitution());
            friction->setValue(rigidBody->getFriction());
            rollingFriction->setValue(rigidBody->getRollingFriction());

            linearDamping->setValue(rigidBody->getLinearDamping());
            angularDamping->setValue(rigidBody->getAngularDamping());

            linearFactorX->setValue(rigidBody->getLinearFactor().X);
            linearFactorY->setValue(rigidBody->getLinearFactor().Y);
            linearFactorZ->setValue(rigidBody->getLinearFactor().Z);
            angularFactorX->setValue(rigidBody->getAngularFactor().X);
            angularFactorY->setValue(rigidBody->getAngularFactor().Y);
            angularFactorZ->setValue(rigidBody->getAngularFactor().Z);

            BodyShapeWidget& bodyShapeWidget = createBodyShapeWidget(rigidBody->getShape(), *objectEntity);
            bodyShapeWidget.setupShapePropertiesFrom(rigidBody->getShape());
            shapeTypeValueLabel->setText(QString::fromStdString(bodyShapeWidget.getBodyShapeName()));
        } else {
            const BodyShapeWidget& bodyShapeWidget = createNoBodyShapeWidget(*objectEntity);
            shapeTypeValueLabel->setText(QString::fromStdString(bodyShapeWidget.getBodyShapeName()));
        }

        disablePhysicsEvent = false;
    }

    BodyShapeWidget& PhysicsWidget::createBodyShapeWidget(const CollisionShape3D& shape, const ObjectEntity& objectEntity) {
        bodyShapeWidget = BodyShapeWidgetRetriever(&objectEntity).createBodyShapeWidget(shape.getShapeType());
        setupBodyShapeWidget();
        return *bodyShapeWidget;
    }

    BodyShapeWidget& PhysicsWidget::createNoBodyShapeWidget(const ObjectEntity& objectEntity) {
        bodyShapeWidget = new NoBodyShapeWidget(&objectEntity);
        setupBodyShapeWidget();
        return *bodyShapeWidget;
    }

    void PhysicsWidget::setupBodyShapeWidget() {
        QString bodyShapeWidgetName = QString::fromStdString("bodyShapeWidget");

        for (int i = 0; i < physicsShapeLayout->count(); ++i) {
            QWidget* oldBodyShapeWidget = physicsShapeLayout->itemAt(i)->widget();
            if(oldBodyShapeWidget && oldBodyShapeWidget->objectName() == bodyShapeWidgetName) {
                physicsShapeLayout->removeWidget(oldBodyShapeWidget);
                oldBodyShapeWidget->deleteLater();
            }
        }

        bodyShapeWidget->setObjectName(bodyShapeWidgetName);
        physicsShapeLayout->addWidget(bodyShapeWidget);
        bodyShapeWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        bodyShapeWidget->show();
        connect(bodyShapeWidget, SIGNAL(bodyShapeChange(std::unique_ptr<const CollisionShape3D>&)), this, SLOT(bodyShapeChanged(std::unique_ptr<const CollisionShape3D>&)));

        notifyObservers(this, OBJECT_BODY_SHAPE_WIDGET_CREATED);
    }

    void PhysicsWidget::showChangeBodyShapeDialog() {
        ChangeBodyShapeDialog changeBodyShapeDialog(this, false, objectEntity->getRigidBody()->getShape().getShapeType());
        changeBodyShapeDialog.exec();

        if (changeBodyShapeDialog.result() == QDialog::Accepted) {
            CollisionShape3D::ShapeType shapeType = changeBodyShapeDialog.getShapeType();
            DefaultShapeQuality defaultShapeQuality = changeBodyShapeDialog.getDefaultShapeQuality();

            objectController->changeBodyShape(*objectEntity, defaultShapeQuality, shapeType);
            setupObjectPhysicsDataFrom(objectEntity->getRigidBody());
        }
    }

    void PhysicsWidget::updateObjectPhysicsProperties() const {
        if (!disablePhysicsEvent) {
            Vector3 linearFactor((float)linearFactorX->value(), (float)linearFactorY->value(), (float)linearFactorZ->value());
            Vector3 angularFactor((float)angularFactorX->value(), (float)angularFactorY->value(), (float)angularFactorZ->value());

            objectController->updateObjectPhysicsProperties(*objectEntity, (float)mass->value(), (float)restitution->value(),
                    (float)friction->value(), (float)rollingFriction->value(), (float)linearDamping->value(), (float)angularDamping->value(),
                    linearFactor, angularFactor);
        }
    }

    void PhysicsWidget::bodyShapeChanged(std::unique_ptr<const CollisionShape3D>& shape) const {
        if (!disablePhysicsEvent) {
            objectController->updateObjectPhysicsShape(*objectEntity, std::move(shape));
        }
    }

}
