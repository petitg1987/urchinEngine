#include <QtWidgets/QHBoxLayout>

#include <panel/objects/ObjectPanelWidget.h>
#include <widget/style/GroupBoxStyleHelper.h>
#include <widget/style/SpinBoxStyleHelper.h>
#include <widget/style/ButtonStyleHelper.h>
#include <widget/style/ComboBoxStyleHelper.h>
#include <widget/style/FrameStyleHelper.h>
#include <panel/objects/dialog/NewObjectDialog.h>
#include <panel/objects/dialog/CloneObjectDialog.h>
#include <panel/objects/dialog/ChangeBodyShapeDialog.h>
#include <panel/objects/bodyshape/BodyShapeWidgetRetriever.h>
#include <scene/SceneDisplayerWindow.h>
#include <scene/objects/move/ObjectMoveController.h>
#include <panel/objects/bodyshape/NoBodyShapeWidget.h>

namespace urchin {

    ObjectPanelWidget::ObjectPanelWidget() :
            objectController(nullptr),
            objectTableView(nullptr),
            addObjectButton(nullptr),
            removeObjectButton(nullptr),
            cloneObjectButton(nullptr),
            tabWidget(nullptr),
            disableObjectEvent(false),
            positionX(nullptr), positionY(nullptr), positionZ(nullptr),
            orientationType(nullptr),
            eulerAxis0(nullptr), eulerAxis1(nullptr), eulerAxis2(nullptr),
            scale(nullptr),
            produceShadowCheckBox(nullptr),
            hasRigidBody(nullptr),
            tabPhysicsRigidBody(nullptr),
            physicsShapeLayout(nullptr),
            mass(nullptr), restitution(nullptr), friction(nullptr), rollingFriction(nullptr),
            linearDamping(nullptr), angularDamping(nullptr),
            linearFactorX(nullptr), linearFactorY(nullptr), linearFactorZ(nullptr),
            angularFactorX(nullptr), angularFactorY(nullptr), angularFactorZ(nullptr),
            shapeTypeValueLabel(nullptr),
            changeBodyShapeButton(nullptr),
            bodyShapeWidget(nullptr) {
        auto* mainLayout = new QVBoxLayout(this);
        mainLayout->setAlignment(Qt::AlignTop);
        mainLayout->setContentsMargins(1, 1, 1, 1);

        objectTableView = new ObjectTableView(this);
        mainLayout->addWidget(objectTableView);
        objectTableView->addObserver(this, ObjectTableView::OBJECT_SELECTION_CHANGED);
        objectTableView->setFixedHeight(220);

        auto* buttonsLayout = new QHBoxLayout();
        mainLayout->addLayout(buttonsLayout);
        buttonsLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

        addObjectButton = new QPushButton("New Object");
        buttonsLayout->addWidget(addObjectButton);
        ButtonStyleHelper::applyNormalStyle(addObjectButton);
        connect(addObjectButton, SIGNAL(clicked()), this, SLOT(showAddObjectDialog()));

        removeObjectButton = new QPushButton("Remove Object");
        buttonsLayout->addWidget(removeObjectButton);
        ButtonStyleHelper::applyNormalStyle(removeObjectButton);
        removeObjectButton->setEnabled(false);
        connect(removeObjectButton, SIGNAL(clicked()), this, SLOT(removeSelectedObject()));

        cloneObjectButton = new QPushButton("Clone Object");
        buttonsLayout->addWidget(cloneObjectButton);
        ButtonStyleHelper::applyNormalStyle(cloneObjectButton);
        cloneObjectButton->setEnabled(false);
        connect(cloneObjectButton, SIGNAL(clicked()), this, SLOT(showCloneObjectDialog()));

        tabWidget = new QTabWidget();
        mainLayout->addWidget(tabWidget);
        tabWidget->hide();

        //general properties
        auto* tabGeneral = new QWidget();
        auto* generalLayout = new QVBoxLayout(tabGeneral);
        generalLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
        generalLayout->setContentsMargins(1, 1, 1, 1);
        setupTransformBox(generalLayout);
        setupFlagsBox(generalLayout);
        tabWidget->addTab(tabGeneral, "General");

        //physics properties
        auto* tabPhysics = new QWidget();
        auto* physicsLayout = new QVBoxLayout(tabPhysics);
        physicsLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
        physicsLayout->setContentsMargins(1, 1, 1, 1);
        setupPhysicsBox(physicsLayout);
        tabWidget->addTab(tabPhysics, "Physics");
    }

    ObjectTableView* ObjectPanelWidget::getObjectTableView() const {
        return objectTableView;
    }

    BodyShapeWidget* ObjectPanelWidget::getBodyShapeWidget() const {
        return bodyShapeWidget.get();
    }

    void ObjectPanelWidget::setupTransformBox(QVBoxLayout* generalLayout) {
        auto* transformGroupBox = new QGroupBox("Transform");
        generalLayout->addWidget(transformGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(transformGroupBox);
        transformGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        auto* transformLayout = new QGridLayout(transformGroupBox);

        setupPosition(transformLayout);
        setupOrientation(transformLayout);
        setupScale(transformLayout);
    }

    void ObjectPanelWidget::setupPosition(QGridLayout* transformLayout) {
        auto* positionLabel= new QLabel("Position:");
        transformLayout->addWidget(positionLabel, 0, 0);

        auto* positionLayout = new QHBoxLayout();
        transformLayout->addLayout(positionLayout, 0, 1);
        positionX = new QDoubleSpinBox();
        positionLayout->addWidget(positionX);
        SpinBoxStyleHelper::applyDefaultStyleOn(positionX);
        connect(positionX, SIGNAL(valueChanged(double)), this, SLOT(updateObjectTransform()));
        positionY = new QDoubleSpinBox();
        positionLayout->addWidget(positionY);
        SpinBoxStyleHelper::applyDefaultStyleOn(positionY);
        connect(positionY, SIGNAL(valueChanged(double)), this, SLOT(updateObjectTransform()));
        positionZ = new QDoubleSpinBox();
        positionLayout->addWidget(positionZ);
        SpinBoxStyleHelper::applyDefaultStyleOn(positionZ);
        connect(positionZ, SIGNAL(valueChanged(double)), this, SLOT(updateObjectTransform()));
    }

    void ObjectPanelWidget::setupOrientation(QGridLayout* transformLayout) {
        auto* orientationTypeLabel = new QLabel("Orient. Type:");
        transformLayout->addWidget(orientationTypeLabel, 1, 0);

        orientationType = new QComboBox();
        transformLayout->addWidget(orientationType, 1, 1);
        ComboBoxStyleHelper::applyOrientationStyleOn(orientationType);
        connect(orientationType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateObjectOrientationType()));

        auto* eulerAngleLabel = new QLabel("Euler Angle:");
        transformLayout->addWidget(eulerAngleLabel, 2, 0);

        auto* eulerAxisLayout = new QHBoxLayout();
        transformLayout->addLayout(eulerAxisLayout, 2, 1);
        eulerAxis0 = new QDoubleSpinBox();
        eulerAxisLayout->addWidget(eulerAxis0);
        SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis0);
        SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis0);
        connect(eulerAxis0, SIGNAL(valueChanged(double)), this, SLOT(updateObjectTransform()));
        eulerAxis1 = new QDoubleSpinBox();
        eulerAxisLayout->addWidget(eulerAxis1);
        SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis1);
        connect(eulerAxis1, SIGNAL(valueChanged(double)), this, SLOT(updateObjectTransform()));
        eulerAxis2 = new QDoubleSpinBox();
        eulerAxisLayout->addWidget(eulerAxis2);
        SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis2);
        connect(eulerAxis2, SIGNAL(valueChanged(double)), this, SLOT(updateObjectTransform()));
    }

    void ObjectPanelWidget::setupScale(QGridLayout* transformLayout) {
        auto* angleLabel = new QLabel("Scale:");
        transformLayout->addWidget(angleLabel, 3, 0);

        scale = new QDoubleSpinBox();
        transformLayout->addWidget(scale, 3, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(scale);
        scale->setMinimum(0.01);
        connect(scale, SIGNAL(valueChanged(double)), this, SLOT(updateObjectScale()));
    }

    void ObjectPanelWidget::setupFlagsBox(QVBoxLayout* generalLayout) {
        auto* flagsGroupBox = new QGroupBox("Flags");
        generalLayout->addWidget(flagsGroupBox);
        GroupBoxStyleHelper::applyNormalStyle(flagsGroupBox);
        flagsGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        auto* flagsLayout = new QGridLayout(flagsGroupBox);

        produceShadowCheckBox = new QCheckBox("Product Shadow");
        flagsLayout->addWidget(produceShadowCheckBox);
        connect(produceShadowCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateObjectFlags()));
    }

    void ObjectPanelWidget::setupPhysicsBox(QVBoxLayout* physicsLayout) {
        hasRigidBody = new QCheckBox("Rigid Body");
        physicsLayout->addWidget(hasRigidBody);
        connect(hasRigidBody, SIGNAL(stateChanged(int)), this, SLOT(rigidBodyToggled(int)));

        tabPhysicsRigidBody = new QTabWidget();
        physicsLayout->addWidget(tabPhysicsRigidBody);

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

    void ObjectPanelWidget::setupPhysicsGeneralPropertiesBox(QVBoxLayout* physicsPropertiesLayout) {
        auto* rigidBodyGeneralBox = new QGroupBox("General");
        physicsPropertiesLayout->addWidget(rigidBodyGeneralBox);
        GroupBoxStyleHelper::applyNormalStyle(rigidBodyGeneralBox);
        rigidBodyGeneralBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        auto* rigidBodyGeneralLayout = new QGridLayout(rigidBodyGeneralBox);

        auto* massLabel = new QLabel("Mass:");
        rigidBodyGeneralLayout->addWidget(massLabel, 0, 0);

        mass = new QDoubleSpinBox();
        rigidBodyGeneralLayout->addWidget(mass, 0, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(mass);
        mass->setMinimum(0.0);
        connect(mass, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));

        auto* restitutionLabel = new QLabel("Restitution:");
        rigidBodyGeneralLayout->addWidget(restitutionLabel, 0, 2);

        restitution = new QDoubleSpinBox();
        rigidBodyGeneralLayout->addWidget(restitution, 0, 3);
        SpinBoxStyleHelper::applyDefaultStyleOn(restitution);
        restitution->setMinimum(0.0);
        restitution->setMaximum(1.0);
        connect(restitution, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));

        auto* frictionLabel = new QLabel("Friction:");
        rigidBodyGeneralLayout->addWidget(frictionLabel, 1, 0);

        friction = new QDoubleSpinBox();
        rigidBodyGeneralLayout->addWidget(friction, 1, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(friction);
        friction->setMinimum(0.0);
        friction->setMaximum(1.0);
        connect(friction, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));

        auto* rollingFrictionLabel = new QLabel("Rolling Friction:");
        rigidBodyGeneralLayout->addWidget(rollingFrictionLabel, 1, 2);

        rollingFriction = new QDoubleSpinBox();
        rigidBodyGeneralLayout->addWidget(rollingFriction, 1, 3);
        SpinBoxStyleHelper::applyDefaultStyleOn(rollingFriction);
        rollingFriction->setMinimum(0.0);
        rollingFriction->setMaximum(1.0);
        connect(rollingFriction, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));
    }

    void ObjectPanelWidget::setupPhysicsDampingPropertiesBox(QVBoxLayout* physicsPropertiesLayout) {
        auto* rigidBodyDampingBox = new QGroupBox("Damping");
        physicsPropertiesLayout->addWidget(rigidBodyDampingBox);
        GroupBoxStyleHelper::applyNormalStyle(rigidBodyDampingBox);
        rigidBodyDampingBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        auto* rigidBodyDampingLayout = new QGridLayout(rigidBodyDampingBox);

        auto* linearDampingLabel = new QLabel("Linear:");
        rigidBodyDampingLayout->addWidget(linearDampingLabel, 0, 0);

        linearDamping = new QDoubleSpinBox();
        rigidBodyDampingLayout->addWidget(linearDamping, 0, 1);
        SpinBoxStyleHelper::applyDefaultStyleOn(linearDamping);
        linearDamping->setMinimum(0.0);
        linearDamping->setMaximum(1.0);
        connect(linearDamping, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));

        auto* angularDampingLabel = new QLabel("Angular:");
        rigidBodyDampingLayout->addWidget(angularDampingLabel, 0, 2);

        angularDamping = new QDoubleSpinBox();
        rigidBodyDampingLayout->addWidget(angularDamping, 0, 3);
        SpinBoxStyleHelper::applyDefaultStyleOn(angularDamping);
        angularDamping->setMinimum(0.0);
        angularDamping->setMaximum(1.0);
        connect(angularDamping, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));
    }

    void ObjectPanelWidget::setupPhysicsFactorPropertiesBox(QVBoxLayout* physicsPropertiesLayout) {
        auto* rigidBodyFactorBox = new QGroupBox("Factor");
        physicsPropertiesLayout->addWidget(rigidBodyFactorBox);
        GroupBoxStyleHelper::applyNormalStyle(rigidBodyFactorBox);
        rigidBodyFactorBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        auto* rigidBodyFactorLayout = new QGridLayout(rigidBodyFactorBox);

        auto* linearFactorLabel = new QLabel("Linear:");
        rigidBodyFactorLayout->addWidget(linearFactorLabel, 0, 0);

        auto* linearFactorLayout = new QHBoxLayout();
        rigidBodyFactorLayout->addLayout(linearFactorLayout, 0, 1);
        linearFactorX = new QDoubleSpinBox();
        linearFactorLayout->addWidget(linearFactorX);
        SpinBoxStyleHelper::applyDefaultStyleOn(linearFactorX);
        linearFactorX->setMinimum(0.0);
        linearFactorX->setMaximum(1.0);
        connect(linearFactorX, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));
        linearFactorY = new QDoubleSpinBox();
        linearFactorLayout->addWidget(linearFactorY);
        SpinBoxStyleHelper::applyDefaultStyleOn(linearFactorY);
        linearFactorY->setMinimum(0.0);
        linearFactorY->setMaximum(1.0);
        connect(linearFactorY, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));
        linearFactorZ = new QDoubleSpinBox();
        linearFactorLayout->addWidget(linearFactorZ);
        SpinBoxStyleHelper::applyDefaultStyleOn(linearFactorZ);
        linearFactorZ->setMinimum(0.0);
        linearFactorZ->setMaximum(1.0);
        connect(linearFactorZ, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));

        auto* angularFactorLabel = new QLabel("Angular:");
        rigidBodyFactorLayout->addWidget(angularFactorLabel, 1, 0);

        auto* angularFactorLayout = new QHBoxLayout();
        rigidBodyFactorLayout->addLayout(angularFactorLayout, 1, 1);
        angularFactorX = new QDoubleSpinBox();
        angularFactorLayout->addWidget(angularFactorX);
        SpinBoxStyleHelper::applyDefaultStyleOn(angularFactorX);
        angularFactorX->setMinimum(0.0);
        angularFactorX->setMaximum(1.0);
        connect(angularFactorX, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));
        angularFactorY = new QDoubleSpinBox();
        angularFactorLayout->addWidget(angularFactorY);
        SpinBoxStyleHelper::applyDefaultStyleOn(angularFactorY);
        angularFactorY->setMinimum(0.0);
        angularFactorY->setMaximum(1.0);
        connect(angularFactorY, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));
        angularFactorZ = new QDoubleSpinBox();
        angularFactorLayout->addWidget(angularFactorZ);
        SpinBoxStyleHelper::applyDefaultStyleOn(angularFactorZ);
        angularFactorZ->setMinimum(0.0);
        angularFactorZ->setMaximum(1.0);
        connect(angularFactorZ, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));
    }

    void ObjectPanelWidget::setupPhysicsShapeBox(QVBoxLayout* physicsShapeLayout) {
        auto* shapeTypeLayout = new QHBoxLayout();
        shapeTypeLayout->setAlignment(Qt::AlignLeft);
        shapeTypeLayout->setSpacing(15);
        physicsShapeLayout->addLayout(shapeTypeLayout);

        auto* shapeTypeLabel = new QLabel("Shape Type:");
        shapeTypeLayout->addWidget(shapeTypeLabel);

        shapeTypeValueLabel = new QLabel();
        shapeTypeLayout->addWidget(shapeTypeValueLabel);

        changeBodyShapeButton = new QPushButton("Change");
        shapeTypeLayout->addWidget(changeBodyShapeButton);
        ButtonStyleHelper::applyNormalStyle(changeBodyShapeButton);
        connect(changeBodyShapeButton, SIGNAL(clicked()), this, SLOT(showChangeBodyShapeDialog()));

        auto* frameLine = new QFrame();
        physicsShapeLayout->addWidget(frameLine);
        FrameStyleHelper::applyLineStyle(frameLine);

        bodyShapeWidget = nullptr;
    }

    void ObjectPanelWidget::load(ObjectController& objectController) {
        this->objectController = &objectController;

        std::list<const SceneObject*> sceneObjects = this->objectController->getSceneObjects();
        for (auto& sceneObject : sceneObjects) {
            objectTableView->addObject(*sceneObject);
        }
    }

    void ObjectPanelWidget::unload() {
        objectTableView->removeAllObjects();

        objectController = nullptr;
    }

    void ObjectPanelWidget::notify(Observable* observable, int notificationType) {
        if (auto* objectTableView = dynamic_cast<ObjectTableView*>(observable)) {
            if (notificationType == ObjectTableView::OBJECT_SELECTION_CHANGED) {
                if (objectTableView->hasSceneObjectSelected()) {
                    const SceneObject& sceneObject = *objectTableView->getSelectedSceneObject();
                    setupObjectDataFrom(sceneObject);

                    removeObjectButton->setEnabled(true);
                    cloneObjectButton->setEnabled(true);
                    tabWidget->show();
                } else {
                    removeObjectButton->setEnabled(false);
                    cloneObjectButton->setEnabled(false);
                    tabWidget->hide();
                }
            }
        } else if (auto* sceneDisplayerWidget = dynamic_cast<SceneDisplayerWindow*>(observable)) {
            if (notificationType == SceneDisplayerWindow::BODY_PICKED) {
                const std::string& bodyId = sceneDisplayerWidget->getLastPickedBodyId();
                const SceneObject* sceneObject = bodyId.empty() ? nullptr : objectController->findSceneObjectByBodyId(bodyId);
                if (sceneObject) {
                    int row = this->objectTableView->getSceneObjectRow(sceneObject);
                    if (row >= 0) {
                        this->objectTableView->selectRow(row);
                    }
                } else {
                    this->objectTableView->clearSelection();
                }
            }
        } else if (auto* objectMoveController = dynamic_cast<ObjectMoveController*>(observable)) {
            if (notificationType == ObjectMoveController::OBJECT_MOVED) {
                setupObjectDataFrom(*objectMoveController->getSelectedSceneObject());
            }
        }
    }

    void ObjectPanelWidget::setupObjectDataFrom(const SceneObject& sceneObject) {
        disableObjectEvent = true;
        const Model* model = sceneObject.getModel();
        const Transform<float>& modelTransform = model->getTransform();

        this->positionX->setValue(modelTransform.getPosition().X);
        this->positionY->setValue(modelTransform.getPosition().Y);
        this->positionZ->setValue(modelTransform.getPosition().Z);

        Vector3<float> eulerAngle = modelTransform.getOrientation().toEulerAngle(Quaternion<float>::RotationSequence::XYZ);
        this->eulerAxis0->setValue(AngleConverter<double>::toDegree(eulerAngle[0]));
        this->eulerAxis1->setValue(AngleConverter<double>::toDegree(eulerAngle[1]));
        this->eulerAxis2->setValue(AngleConverter<double>::toDegree(eulerAngle[2]));

        this->scale->setValue(modelTransform.getScale());

        this->produceShadowCheckBox->setChecked(model->isProduceShadow());

        setupObjectPhysicsDataFrom(sceneObject);
        disableObjectEvent = false;
    }

    void ObjectPanelWidget::setupObjectPhysicsDataFrom(const SceneObject& sceneObject) {
        disableObjectEvent = true;
        const RigidBody* rigidBody = sceneObject.getRigidBody();

        if (rigidBody) {
            hasRigidBody->setChecked(true);
            tabPhysicsRigidBody->show();

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

            BodyShapeWidget& bodyShapeWidget = createBodyShapeWidget(rigidBody->getShape(), sceneObject);
            bodyShapeWidget.setupShapePropertiesFrom(rigidBody->getShape());
            shapeTypeValueLabel->setText(QString::fromStdString(bodyShapeWidget.getBodyShapeName()));
        } else {
            hasRigidBody->setChecked(false);
            tabPhysicsRigidBody->hide();

            BodyShapeWidget& bodyShapeWidget = createNoBodyShapeWidget(sceneObject);
            shapeTypeValueLabel->setText(QString::fromStdString(bodyShapeWidget.getBodyShapeName()));
        }

        disableObjectEvent = false;
    }

    BodyShapeWidget& ObjectPanelWidget::createBodyShapeWidget(const CollisionShape3D& shape, const SceneObject& sceneObject) {
        bodyShapeWidget = BodyShapeWidgetRetriever(&sceneObject).createBodyShapeWidget(shape.getShapeType());
        setupBodyShapeWidget();
        return *bodyShapeWidget;
    }

    BodyShapeWidget& ObjectPanelWidget::createNoBodyShapeWidget(const SceneObject& sceneObject) {
        bodyShapeWidget = std::make_unique<NoBodyShapeWidget>(&sceneObject);
        setupBodyShapeWidget();
        return *bodyShapeWidget;
    }

    void ObjectPanelWidget::setupBodyShapeWidget() {
        physicsShapeLayout->addWidget(bodyShapeWidget.get());
        bodyShapeWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        bodyShapeWidget->show();
        connect(bodyShapeWidget.get(), SIGNAL(bodyShapeChange(std::unique_ptr<const CollisionShape3D>&)), this, SLOT(bodyShapeChanged(std::unique_ptr<const CollisionShape3D>&)));

        notifyObservers(this, NotificationType::OBJECT_BODY_SHAPE_WIDGET_CREATED);
    }

    void ObjectPanelWidget::showAddObjectDialog() {
        NewObjectDialog newSceneObjectDialog(this, objectController);
        newSceneObjectDialog.exec();

        if (newSceneObjectDialog.result() == QDialog::Accepted) {
            std::unique_ptr<SceneObject> sceneObject = newSceneObjectDialog.moveSceneObject();
            SceneObject* sceneObjectPtr = sceneObject.get();
            objectController->addSceneObject(std::move(sceneObject));
            objectController->createDefaultBody(*sceneObjectPtr);

            int row = objectTableView->addObject(*sceneObjectPtr);
            objectTableView->selectRow(row);
        }
    }

    void ObjectPanelWidget::removeSelectedObject() {
        if (objectTableView->hasSceneObjectSelected()) {
            const SceneObject& sceneObject = *objectTableView->getSelectedSceneObject();
            objectController->removeSceneObject(sceneObject);

            objectTableView->removeSelectedObject();
        }
    }

    void ObjectPanelWidget::showCloneObjectDialog() {
        CloneObjectDialog cloneSceneObjectDialog(this, objectController);
        cloneSceneObjectDialog.exec();

        if (cloneSceneObjectDialog.result() == QDialog::Accepted) {
            std::unique_ptr<SceneObject> newSceneObject = cloneSceneObjectDialog.moveSceneObject();
            SceneObject *newSceneObjectPtr = newSceneObject.get();
            const SceneObject& toCloneSceneObject = *objectTableView->getSelectedSceneObject();
            objectController->cloneSceneObject(std::move(newSceneObject), toCloneSceneObject);

            int row = objectTableView->addObject(*newSceneObjectPtr);
            objectTableView->selectRow(row);
        }
    }

    void ObjectPanelWidget::updateObjectOrientationType() {
        if (!disableObjectEvent) {
            const SceneObject* sceneObject = objectTableView->getSelectedSceneObject();

            QVariant variant = orientationType->currentData();
            auto newRotationSequence = static_cast<Quaternion<float>::RotationSequence>(variant.toInt());

            Quaternion<float> orientation = sceneObject->getModel()->getTransform().getOrientation();
            Vector3<float> eulerAngle = orientation.toEulerAngle(newRotationSequence);

            eulerAxis0->setValue(AngleConverter<float>::toDegree(eulerAngle.X));
            eulerAxis1->setValue(AngleConverter<float>::toDegree(eulerAngle.Y));
            eulerAxis2->setValue(AngleConverter<float>::toDegree(eulerAngle.Z));

            updateObjectTransform();
        }
    }

    void ObjectPanelWidget::updateObjectTransform() {
        if (!disableObjectEvent) {
            const SceneObject& sceneObject = *objectTableView->getSelectedSceneObject();

            Vector3<float> eulerAngle(
                    AngleConverter<float>::toRadian((float)eulerAxis0->value()),
                    AngleConverter<float>::toRadian((float)eulerAxis1->value()),
                    AngleConverter<float>::toRadian((float)eulerAxis2->value())
            );

            QVariant variant = orientationType->currentData();
            auto rotationSequence = static_cast<Quaternion<float>::RotationSequence>(variant.toInt());

            Transform<float> newSceneObjectTransform(
                    Point3<float>((float)positionX->value(),(float)positionY->value(),(float)positionZ->value()),
                    Quaternion<float>(eulerAngle, rotationSequence),
                    (float)scale->value());

            objectController->updateSceneObjectTransform(sceneObject, newSceneObjectTransform);
        }
    }

    void ObjectPanelWidget::updateObjectScale() {
        if (!disableObjectEvent) {
            updateObjectTransform();

            const SceneObject& sceneObject = *objectTableView->getSelectedSceneObject();
            if (sceneObject.getRigidBody()) {
                const CollisionShape3D& updatedCollisionShape = sceneObject.getRigidBody()->getShape();
                auto& bodyShapeWidget = createBodyShapeWidget(updatedCollisionShape, sceneObject);
                bodyShapeWidget.setupShapePropertiesFrom(updatedCollisionShape);
            }
        }
    }

    void ObjectPanelWidget::updateObjectFlags() {
        if (!disableObjectEvent) {
            const SceneObject& sceneObject = *objectTableView->getSelectedSceneObject();

            bool produceShadow = produceShadowCheckBox->checkState() == Qt::Checked;
            objectController->updateSceneObjectFlags(sceneObject, produceShadow);
        }
    }

    void ObjectPanelWidget::showChangeBodyShapeDialog() {
        ChangeBodyShapeDialog changeBodyShapeDialog(this, false);
        changeBodyShapeDialog.exec();

        if (changeBodyShapeDialog.result() == QDialog::Accepted) {
            const SceneObject& sceneObject = *objectTableView->getSelectedSceneObject();
            CollisionShape3D::ShapeType shapeType = changeBodyShapeDialog.getShapeType();

            objectController->changeBodyShape(sceneObject, shapeType);
            setupObjectPhysicsDataFrom(sceneObject);
        }
    }

    void ObjectPanelWidget::rigidBodyToggled(int rigidBodyState) {
        if (!disableObjectEvent) {
            const SceneObject& sceneObject = *objectTableView->getSelectedSceneObject();
            if (Qt::CheckState::Checked == rigidBodyState) {
                tabPhysicsRigidBody->show();
                objectController->createDefaultBody(sceneObject);
            } else if (Qt::CheckState::Unchecked == rigidBodyState) {
                objectController->removeBody(sceneObject);
                tabPhysicsRigidBody->hide();
            }

            setupObjectPhysicsDataFrom(sceneObject);
        }
    }

    void ObjectPanelWidget::updateObjectPhysicsProperties() {
        if (!disableObjectEvent) {
            const SceneObject& sceneObject = *objectTableView->getSelectedSceneObject();

            Vector3<float> linearFactor((float)linearFactorX->value(), (float)linearFactorY->value(), (float)linearFactorZ->value());
            Vector3<float> angularFactor((float)angularFactorX->value(), (float)angularFactorY->value(), (float)angularFactorZ->value());

            objectController->updateSceneObjectPhysicsProperties(sceneObject, (float)mass->value(), (float)restitution->value(),
                    (float)friction->value(), (float)rollingFriction->value(), (float)linearDamping->value(), (float)angularDamping->value(),
                    linearFactor, angularFactor);
        }
    }

    void ObjectPanelWidget::bodyShapeChanged(std::unique_ptr<const CollisionShape3D>& shape) {
        if (!disableObjectEvent) {
            const SceneObject& sceneObject = *objectTableView->getSelectedSceneObject();
            objectController->updateSceneObjectPhysicsShape(sceneObject, std::move(shape));
        }
    }

}
