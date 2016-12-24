#include <QtWidgets/QHBoxLayout>

#include "ObjectControllerWidget.h"
#include "support/GroupBoxStyleHelper.h"
#include "support/SpinBoxStyleHelper.h"
#include "support/ButtonStyleHelper.h"
#include "support/ComboBoxStyleHelper.h"
#include "support/FrameStyleHelper.h"
#include "scene/controller/objects/dialog/NewObjectDialog.h"
#include "scene/controller/objects/dialog/ChangeBodyShapeDialog.h"
#include "scene/controller/objects/bodyshape/BodyShapeWidgetRetriever.h"

namespace urchin
{

	ObjectControllerWidget::ObjectControllerWidget() :
			objectController(nullptr),
			disableObjectEvent(false)
	{
		QVBoxLayout *mainLayout = new QVBoxLayout(this);
		mainLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
		mainLayout->setContentsMargins(1, 1, 1, 1);

		objectTableView = new ObjectTableView();
		mainLayout->addWidget(objectTableView);
		objectTableView->addObserver(this, ObjectTableView::SELECTION_CHANGED);
		objectTableView->setFixedHeight(220);

		QHBoxLayout *buttonsLayout = new QHBoxLayout();
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

		tabWidget = new QTabWidget();
		mainLayout->addWidget(tabWidget);
		tabWidget->hide();

		//general properties
		QWidget *tabGeneral = new QWidget();
		QVBoxLayout *generalLayout = new QVBoxLayout(tabGeneral);
		generalLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
		generalLayout->setContentsMargins(1, 1, 1, 1);
		setupTransformBox(generalLayout);
		setupFlagsBox(generalLayout);
		tabWidget->addTab(tabGeneral, "General");

		//physics properties
		QWidget *tabPhysics = new QWidget();
		QVBoxLayout *physicsLayout = new QVBoxLayout(tabPhysics);
		physicsLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
		physicsLayout->setContentsMargins(1, 1, 1, 1);
		setupPhysicsBox(physicsLayout);
		tabWidget->addTab(tabPhysics, "Physics");
	}

	ObjectControllerWidget::~ObjectControllerWidget()
	{

	}

	ObjectTableView *ObjectControllerWidget::getObjectTableView() const
	{
		return objectTableView;
	}

	BodyShapeWidget *ObjectControllerWidget::getBodyShapeWidget() const
	{
		return bodyShapeWidget;
	}

	void ObjectControllerWidget::setupTransformBox(QVBoxLayout *generalLayout)
	{
		QGroupBox *transformGroupBox = new QGroupBox("Transform");
		generalLayout->addWidget(transformGroupBox);
		GroupBoxStyleHelper::applyNormalStyle(transformGroupBox);
		transformGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

		QGridLayout *transformLayout = new QGridLayout(transformGroupBox);

		setupPosition(transformLayout);
		setupOrientation(transformLayout);
		setupScale(transformLayout);
	}

	void ObjectControllerWidget::setupPosition(QGridLayout *transformLayout)
	{
		QLabel *positionLabel= new QLabel("Position:");
		transformLayout->addWidget(positionLabel, 0, 0);

		QHBoxLayout *positionLayout = new QHBoxLayout();
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

	void ObjectControllerWidget::setupOrientation(QGridLayout *transformLayout)
	{
		QLabel *orientationTypeLabel = new QLabel("Orient. Type:");
		transformLayout->addWidget(orientationTypeLabel, 1, 0);

		orientationType = new QComboBox();
		transformLayout->addWidget(orientationType, 1, 1);
		ComboBoxStyleHelper::applyOrientationStyleOn(orientationType);
		connect(orientationType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateObjectOrientationType()));

		QLabel *eulerAngleLabel = new QLabel("Euler Angle:");
		transformLayout->addWidget(eulerAngleLabel, 2, 0);

		QHBoxLayout *eulerAxisLayout = new QHBoxLayout();
		transformLayout->addLayout(eulerAxisLayout, 2, 1);
		eulerAxis0 = new QDoubleSpinBox();
		eulerAxisLayout->addWidget(eulerAxis0);
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

	void ObjectControllerWidget::setupScale(QGridLayout *transformLayout)
	{
		QLabel *angleLabel = new QLabel("Scale:");
		transformLayout->addWidget(angleLabel, 3, 0);

		scale = new QDoubleSpinBox();
		transformLayout->addWidget(scale, 3, 1);
		SpinBoxStyleHelper::applyDefaultStyleOn(scale);
		scale->setMinimum(0.0);
		connect(scale, SIGNAL(valueChanged(double)), this, SLOT(updateObjectScale()));
	}

	void ObjectControllerWidget::setupFlagsBox(QVBoxLayout *generalLayout)
	{
		QGroupBox *flagsGroupBox = new QGroupBox("Flags");
		generalLayout->addWidget(flagsGroupBox);
		GroupBoxStyleHelper::applyNormalStyle(flagsGroupBox);
		flagsGroupBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

		QGridLayout *flagsLayout = new QGridLayout(flagsGroupBox);

		produceShadowCheckBox = new QCheckBox("Product Shadow");
		flagsLayout->addWidget(produceShadowCheckBox);
		connect(produceShadowCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateObjectFlags()));
	}

	void ObjectControllerWidget::setupPhysicsBox(QVBoxLayout *physicsLayout)
	{
		hasRigidBody = new QCheckBox("Rigid Body");
		physicsLayout->addWidget(hasRigidBody);
		connect(hasRigidBody, SIGNAL(stateChanged(int)), this, SLOT(rigidBodyToggled(int)));

		tabPhysicsRigidBody = new QTabWidget();
		physicsLayout->addWidget(tabPhysicsRigidBody);

		QWidget *tabPhysicsProperties = new QWidget();
		QVBoxLayout *physicsPropertiesLayout = new QVBoxLayout(tabPhysicsProperties);
		physicsPropertiesLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
		physicsPropertiesLayout->setContentsMargins(1, 1, 1, 1);
		setupPhysicsGeneralPropertiesBox(physicsPropertiesLayout);
		setupPhysicsDampingPropertiesBox(physicsPropertiesLayout);
		setupPhysicsFactorPropertiesBox(physicsPropertiesLayout);
		tabPhysicsRigidBody->addTab(tabPhysicsProperties, "Properties");

		QWidget *tabPhysicsShape = new QWidget();
		physicsShapeLayout = new QVBoxLayout(tabPhysicsShape);
		physicsShapeLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
		physicsShapeLayout->setContentsMargins(1, 1, 1, 1);
		setupPhysicsShapeBox(physicsShapeLayout);
		tabPhysicsRigidBody->addTab(tabPhysicsShape, "Shape");
	}

	void ObjectControllerWidget::setupPhysicsGeneralPropertiesBox(QVBoxLayout *physicsPropertiesLayout)
	{
		QGroupBox *rigidBodyGeneralBox = new QGroupBox("General");
		physicsPropertiesLayout->addWidget(rigidBodyGeneralBox);
		GroupBoxStyleHelper::applyNormalStyle(rigidBodyGeneralBox);
		rigidBodyGeneralBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

		QGridLayout *rigidBodyGeneralLayout = new QGridLayout(rigidBodyGeneralBox);

		QLabel *massLabel = new QLabel("Mass:");
		rigidBodyGeneralLayout->addWidget(massLabel, 0, 0);

		mass = new QDoubleSpinBox();
		rigidBodyGeneralLayout->addWidget(mass, 0, 1);
		SpinBoxStyleHelper::applyDefaultStyleOn(mass);
		mass->setMinimum(0.0);
		connect(mass, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));

		QLabel *restitutionLabel = new QLabel("Restitution:");
		rigidBodyGeneralLayout->addWidget(restitutionLabel, 0, 2);

		restitution = new QDoubleSpinBox();
		rigidBodyGeneralLayout->addWidget(restitution, 0, 3);
		SpinBoxStyleHelper::applyDefaultStyleOn(restitution);
		restitution->setMinimum(0.0);
		restitution->setMaximum(1.0);
		connect(restitution, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));

		QLabel *frictionLabel = new QLabel("Friction:");
		rigidBodyGeneralLayout->addWidget(frictionLabel, 1, 0);

		friction = new QDoubleSpinBox();
		rigidBodyGeneralLayout->addWidget(friction, 1, 1);
		SpinBoxStyleHelper::applyDefaultStyleOn(friction);
		friction->setMinimum(0.0);
		friction->setMaximum(1.0);
		connect(friction, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));

		QLabel *rollingFrictionLabel = new QLabel("Rolling Friction:");
		rigidBodyGeneralLayout->addWidget(rollingFrictionLabel, 1, 2);

		rollingFriction = new QDoubleSpinBox();
		rigidBodyGeneralLayout->addWidget(rollingFriction, 1, 3);
		SpinBoxStyleHelper::applyDefaultStyleOn(rollingFriction);
		rollingFriction->setMinimum(0.0);
		rollingFriction->setMaximum(1.0);
		connect(rollingFriction, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));
	}

	void ObjectControllerWidget::setupPhysicsDampingPropertiesBox(QVBoxLayout *physicsPropertiesLayout)
	{
		QGroupBox *rigidBodyDampingBox = new QGroupBox("Damping");
		physicsPropertiesLayout->addWidget(rigidBodyDampingBox);
		GroupBoxStyleHelper::applyNormalStyle(rigidBodyDampingBox);
		rigidBodyDampingBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

		QGridLayout *rigidBodyDampingLayout = new QGridLayout(rigidBodyDampingBox);

		QLabel *linearDampingLabel = new QLabel("Linear:");
		rigidBodyDampingLayout->addWidget(linearDampingLabel, 0, 0);

		linearDamping = new QDoubleSpinBox();
		rigidBodyDampingLayout->addWidget(linearDamping, 0, 1);
		SpinBoxStyleHelper::applyDefaultStyleOn(linearDamping);
		linearDamping->setMinimum(0.0);
		linearDamping->setMaximum(1.0);
		connect(linearDamping, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));

		QLabel *angularDampingLabel = new QLabel("Angular:");
		rigidBodyDampingLayout->addWidget(angularDampingLabel, 0, 2);

		angularDamping = new QDoubleSpinBox();
		rigidBodyDampingLayout->addWidget(angularDamping, 0, 3);
		SpinBoxStyleHelper::applyDefaultStyleOn(angularDamping);
		angularDamping->setMinimum(0.0);
		angularDamping->setMaximum(1.0);
		connect(angularDamping, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));
	}

	void ObjectControllerWidget::setupPhysicsFactorPropertiesBox(QVBoxLayout *physicsPropertiesLayout)
	{
		QGroupBox *rigidBodyFactorBox = new QGroupBox("Factor");
		physicsPropertiesLayout->addWidget(rigidBodyFactorBox);
		GroupBoxStyleHelper::applyNormalStyle(rigidBodyFactorBox);
		rigidBodyFactorBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

		QGridLayout *rigidBodyFactorLayout = new QGridLayout(rigidBodyFactorBox);

		QLabel *linearFactorLabel = new QLabel("Linear:");
		rigidBodyFactorLayout->addWidget(linearFactorLabel, 0, 0);

		QHBoxLayout *linearFactorLayout = new QHBoxLayout();
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

		QLabel *angularFactorLabel = new QLabel("Angular:");
		rigidBodyFactorLayout->addWidget(angularFactorLabel, 1, 0);

		QHBoxLayout *angularFactorLayout = new QHBoxLayout();
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

	void ObjectControllerWidget::setupPhysicsShapeBox(QVBoxLayout *physicsShapeLayout)
	{
		QHBoxLayout *shapeTypeLayout = new QHBoxLayout();
		shapeTypeLayout->setAlignment(Qt::AlignLeft);
		shapeTypeLayout->setSpacing(15);
		physicsShapeLayout->addLayout(shapeTypeLayout);

		QLabel *shapeTypeLabel = new QLabel("Shape Type:");
		shapeTypeLayout->addWidget(shapeTypeLabel);

		shapeTypeValueLabel = new QLabel();
		shapeTypeLayout->addWidget(shapeTypeValueLabel);

		changeBodyShapeButton = new QPushButton("Change");
		shapeTypeLayout->addWidget(changeBodyShapeButton);
		ButtonStyleHelper::applyNormalStyle(changeBodyShapeButton);
		connect(changeBodyShapeButton, SIGNAL(clicked()), this, SLOT(showChangeBodyShapeDialog()));

		QFrame *frameLine = new QFrame();
		physicsShapeLayout->addWidget(frameLine);
		FrameStyleHelper::applyLineStyle(frameLine);

		bodyShapeWidget = nullptr;
	}

	void ObjectControllerWidget::load(ObjectController *objectController)
	{
		this->objectController = objectController;

		std::list<const SceneObject *> sceneObjects = objectController->getSceneObjects();
		for(std::list<const SceneObject *>::const_iterator it=sceneObjects.begin(); it!=sceneObjects.end(); ++it)
		{
			objectTableView->addObject((*it));
		}
	}

	void ObjectControllerWidget::unload()
	{
		objectTableView->removeAllObjects();

		objectController = nullptr;
	}

	void ObjectControllerWidget::notify(Observable *observable, int notificationType)
	{
		if(ObjectTableView *objectTableView = dynamic_cast<ObjectTableView *>(observable))
		{
			switch(notificationType)
			{
				case ObjectTableView::SELECTION_CHANGED:
					if(objectTableView->hasSceneObjectSelected())
					{
						const SceneObject *sceneObject = objectTableView->getSelectedSceneObject();
						setupObjectDataFrom(sceneObject);

						removeObjectButton->setEnabled(true);
						tabWidget->show();
					}else
					{
						removeObjectButton->setEnabled(false);
						tabWidget->hide();
					}
					break;
			}
		}
	}

	void ObjectControllerWidget::setupObjectDataFrom(const SceneObject *sceneObject)
	{
		disableObjectEvent = true;
		const Model *model = sceneObject->getModel();
		const Transform<float> &modelTransform = model->getTransform();

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

	void ObjectControllerWidget::setupObjectPhysicsDataFrom(const SceneObject *sceneObject)
	{
		const RigidBody *rigidBody = sceneObject->getRigidBody();
		std::shared_ptr<const CollisionShape3D> bodyScaledShape(nullptr);
		if(rigidBody!=nullptr)
		{
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

			bodyScaledShape = rigidBody->getScaledShape();
		}else
		{
			hasRigidBody->setChecked(false);
			tabPhysicsRigidBody->hide();
		}

		BodyShapeWidget *bodyShapeWidget = retrieveBodyShapeWidget(bodyScaledShape, sceneObject);
		bodyShapeWidget->setupShapePropertiesFrom(bodyScaledShape);

		shapeTypeValueLabel->setText(QString::fromStdString(bodyShapeWidget->getBodyShapeName()));
	}

	BodyShapeWidget *ObjectControllerWidget::retrieveBodyShapeWidget(std::shared_ptr<const CollisionShape3D> shape, const SceneObject *sceneObject)
	{
		delete bodyShapeWidget;

		bodyShapeWidget = BodyShapeWidgetRetriever(sceneObject).retrieveShapeWidget(shape);
		physicsShapeLayout->addWidget(bodyShapeWidget);
		bodyShapeWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
		bodyShapeWidget->show();
		connect(bodyShapeWidget, SIGNAL(bodyShapeChange(std::shared_ptr<const CollisionShape3D>)), this, SLOT(bodyShapeChanged(std::shared_ptr<const CollisionShape3D>)));

		notifyObservers(this, NotificationType::BODY_SHAPE_INITIALIZED);

		return bodyShapeWidget;
	}

	void ObjectControllerWidget::showAddObjectDialog()
	{
		NewObjectDialog newSceneObjectDialog(this, objectController);
		newSceneObjectDialog.exec();

		if(newSceneObjectDialog.result()==QDialog::Accepted)
		{
			SceneObject *sceneObject = newSceneObjectDialog.getSceneObject();
			objectController->addSceneObject(sceneObject);

			objectTableView->addObject(sceneObject);
		}
	}

	void ObjectControllerWidget::removeSelectedObject()
	{
		if(objectTableView->hasSceneObjectSelected())
		{
			const SceneObject *sceneObject = objectTableView->getSelectedSceneObject();
			objectController->removeSceneObject(sceneObject);

			objectTableView->removeSelectedObject();
		}
	}

	void ObjectControllerWidget::updateObjectOrientationType()
	{
		if(!disableObjectEvent)
		{
			const SceneObject *sceneObject = objectTableView->getSelectedSceneObject();

			QVariant variant = orientationType->currentData();
			Quaternion<float>::RotationSequence newRotationSequence = static_cast<Quaternion<float>::RotationSequence>(variant.toInt());

			Quaternion<float> orientation = sceneObject->getModel()->getTransform().getOrientation();
			Vector3<float> eulerAngle = orientation.toEulerAngle(newRotationSequence);

			eulerAxis0->setValue(AngleConverter<float>::toDegree(eulerAngle.X));
			eulerAxis1->setValue(AngleConverter<float>::toDegree(eulerAngle.Y));
			eulerAxis2->setValue(AngleConverter<float>::toDegree(eulerAngle.Z));

			updateObjectTransform();
		}
	}

	void ObjectControllerWidget::updateObjectTransform()
	{
		if(!disableObjectEvent)
		{
			const SceneObject *sceneObject = objectTableView->getSelectedSceneObject();

			Vector3<float> eulerAngle(
					AngleConverter<float>::toRadian(eulerAxis0->value()),
					AngleConverter<float>::toRadian(eulerAxis1->value()),
					AngleConverter<float>::toRadian(eulerAxis2->value())
			);

			QVariant variant = orientationType->currentData();
			Quaternion<float>::RotationSequence rotationSequence = static_cast<Quaternion<float>::RotationSequence>(variant.toInt());

			Transform<float> newSceneObjectTransform(
					Point3<float>(positionX->value(), positionY->value(), positionZ->value()),
					Quaternion<float>(eulerAngle, rotationSequence),
					scale->value());

			objectController->updateSceneObjectTransform(sceneObject, newSceneObjectTransform);
		}
	}

	void ObjectControllerWidget::updateObjectScale()
	{
		if(!disableObjectEvent)
		{
			updateObjectTransform();

			const SceneObject *sceneObject = objectTableView->getSelectedSceneObject();
			if(sceneObject->getRigidBody()!=nullptr)
			{
				std::shared_ptr<const CollisionShape3D> originalCollisionShape = sceneObject->getRigidBody()->getOriginalShape();
				const SceneObject *updatedSceneObject = objectController->updateSceneObjectPhysicsShape(sceneObject, originalCollisionShape);

				std::shared_ptr<const CollisionShape3D> scaledCollisionShape = updatedSceneObject->getRigidBody()->getScaledShape();
				BodyShapeWidget *bodyShapeWidget = retrieveBodyShapeWidget(scaledCollisionShape, updatedSceneObject);
				bodyShapeWidget->setupShapePropertiesFrom(scaledCollisionShape);
			}
		}
	}

	void ObjectControllerWidget::updateObjectFlags()
	{
		if(!disableObjectEvent)
		{
			const SceneObject *sceneObject = objectTableView->getSelectedSceneObject();

			bool produceShadow = produceShadowCheckBox->checkState()==Qt::Checked;
			objectController->updateSceneObjectFlags(sceneObject, produceShadow);
		}
	}

	void ObjectControllerWidget::showChangeBodyShapeDialog()
	{
		ChangeBodyShapeDialog changeBodyShapeDialog(this, false);
		changeBodyShapeDialog.exec();

		if(changeBodyShapeDialog.result()==QDialog::Accepted)
		{
			const SceneObject *sceneObject = objectTableView->getSelectedSceneObject();
			CollisionShape3D::ShapeType shapeType = changeBodyShapeDialog.getShapeType();

			objectController->changeBodyShape(sceneObject, shapeType);
			setupObjectPhysicsDataFrom(sceneObject);
		}
	}

	void ObjectControllerWidget::rigidBodyToggled(int rigidBodyState)
	{
		if(!disableObjectEvent)
		{
			const SceneObject *sceneObject = objectTableView->getSelectedSceneObject();
			if(Qt::CheckState::Checked==rigidBodyState)
			{
				tabPhysicsRigidBody->show();
				objectController->createDefaultBody(sceneObject);
			}else if(Qt::CheckState::Unchecked==rigidBodyState)
			{
				objectController->removeBody(sceneObject);
				tabPhysicsRigidBody->hide();
			}

			setupObjectPhysicsDataFrom(sceneObject);
		}
	}

	void ObjectControllerWidget::updateObjectPhysicsProperties()
	{
		if(!disableObjectEvent)
		{
			const SceneObject *sceneObject = objectTableView->getSelectedSceneObject();

			Vector3<float> linearFactor(linearFactorX->value(), linearFactorY->value(), linearFactorZ->value());
			Vector3<float> angularFactor(angularFactorX->value(), angularFactorY->value(), angularFactorZ->value());

			objectController->updateSceneObjectPhysicsProperties(sceneObject, mass->value(), restitution->value(),
					friction->value(), rollingFriction->value(), linearDamping->value(), angularDamping->value(),
					linearFactor, angularFactor);
		}
	}

	void ObjectControllerWidget::bodyShapeChanged(std::shared_ptr<const CollisionShape3D> scaledShape)
	{
		if(!disableObjectEvent)
		{
			const SceneObject *sceneObject = objectTableView->getSelectedSceneObject();

			float invScale = 1.0 / sceneObject->getModel()->getTransform().getScale();
			std::shared_ptr<const CollisionShape3D> originalShape = scaledShape->scale(invScale);

			objectController->updateSceneObjectPhysicsShape(sceneObject, originalShape);
		}
	}

}
