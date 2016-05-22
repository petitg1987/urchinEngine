#include "ObjectControllerWidget.h"
#include "support/GroupBoxStyleHelper.h"
#include "support/SpinBoxStyleHelper.h"
#include "scene/controller/objects/dialog/NewObjectDialog.h"
#include "scene/controller/objects/dialog/ChangeBodyShapeDialog.h"
#include "scene/controller/objects/bodyshape/BodyShapeWidgetRetriever.h"

namespace urchin
{

	ObjectControllerWidget::ObjectControllerWidget() :
			objectController(nullptr),
			disableObjectEvent(false)
	{
		objectTableView = new ObjectTableView(this);
		objectTableView->addObserver(this, ObjectTableView::SELECTION_CHANGED);
		objectTableView->setGeometry(QRect(0, 0, 355, 220));

		addObjectButton = new QPushButton(this);
		addObjectButton->setText("New Object");
		addObjectButton->setGeometry(QRect(0, 221, 90, 22));
		connect(addObjectButton, SIGNAL(clicked()), this, SLOT(showAddObjectDialog()));

		removeObjectButton = new QPushButton(this);
		removeObjectButton->setText("Remove Object");
		removeObjectButton->setGeometry(QRect(91, 221, 90, 22));
		removeObjectButton->setEnabled(false);
		connect(removeObjectButton, SIGNAL(clicked()), this, SLOT(removeSelectedObject()));

		tabWidget = new QTabWidget(this);
		tabWidget->setGeometry(QRect(0, 250, 355, 525));
		tabWidget->hide();

		QWidget *tabGeneral = new QWidget();
		setupTransformBox(tabGeneral);
		setupFlagsBox(tabGeneral);
		tabWidget->addTab(tabGeneral, "General");

		QWidget *tabPhysics = new QWidget();
		setupPhysicsBox(tabPhysics);
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

	void ObjectControllerWidget::setupTransformBox(QWidget *tabGeneral)
	{
		QGroupBox *transformGroupBox = new QGroupBox(tabGeneral);
		transformGroupBox->setTitle("Transform");
		transformGroupBox->setGeometry(QRect(0, 5, 350, 145));
		GroupBoxStyleHelper::applyNormalStyle(transformGroupBox);

		setupPosition(transformGroupBox);
		setupOrientation(transformGroupBox);
		setupScale(transformGroupBox);
	}

	void ObjectControllerWidget::setupPosition(QGroupBox *transformGroupBox)
	{
		QLabel *positionLabel= new QLabel(transformGroupBox);
		positionLabel->setText("Position:");
		positionLabel->setGeometry(QRect(5, 15, 55, 22));

		positionX = new QDoubleSpinBox(transformGroupBox);
		positionX->setGeometry(QRect(85, 15, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(positionX);
		connect(positionX, SIGNAL(valueChanged(double)), this, SLOT(updateObjectTransform()));
		positionY = new QDoubleSpinBox(transformGroupBox);
		positionY->setGeometry(QRect(165, 15, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(positionY);
		connect(positionY, SIGNAL(valueChanged(double)), this, SLOT(updateObjectTransform()));
		positionZ = new QDoubleSpinBox(transformGroupBox);
		positionZ->setGeometry(QRect(245, 15, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(positionZ);
		connect(positionZ, SIGNAL(valueChanged(double)), this, SLOT(updateObjectTransform()));
	}

	void ObjectControllerWidget::setupOrientation(QGroupBox *transformGroupBox)
	{
		QLabel *orientationTypeLabel = new QLabel(transformGroupBox);
		orientationTypeLabel->setText("Orient. Type:");
		orientationTypeLabel->setGeometry(QRect(5, 40, 80, 22));

		orientationType = new QComboBox(transformGroupBox);
		orientationType->setGeometry(QRect(85, 40, 160, 22));
		orientationType->addItem(EULER_XYZ_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::XYZ));
		orientationType->addItem(EULER_XZY_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::XZY));
		orientationType->addItem(EULER_YXZ_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::YXZ));
		orientationType->addItem(EULER_YZX_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::YZX));
		orientationType->addItem(EULER_ZXY_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::ZXY));
		orientationType->addItem(EULER_ZYX_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::ZYX));
		orientationType->addItem(EULER_XYX_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::XYX));
		orientationType->addItem(EULER_XZX_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::XZX));
		orientationType->addItem(EULER_YXY_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::YXY));
		orientationType->addItem(EULER_YZY_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::YZY));
		orientationType->addItem(EULER_ZXZ_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::ZXZ));
		orientationType->addItem(EULER_ZYZ_ORIENT_LABEL, QVariant(Quaternion<float>::RotationSequence::ZYZ));
		connect(orientationType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateObjectOrientationType()));

		QLabel *orientationAxisLabel = new QLabel(transformGroupBox);
		orientationAxisLabel->setText("Euler Angle:");
		orientationAxisLabel->setGeometry(QRect(5, 65, 80, 22));

		eulerAxis0 = new QDoubleSpinBox(transformGroupBox);
		eulerAxis0->setGeometry(QRect(85, 65, 80, 22));
		SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis0);
		connect(eulerAxis0, SIGNAL(valueChanged(double)), this, SLOT(updateObjectTransform()));

		eulerAxis1 = new QDoubleSpinBox(transformGroupBox);
		eulerAxis1->setGeometry(QRect(165, 65, 80, 22));
		SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis1);
		connect(eulerAxis1, SIGNAL(valueChanged(double)), this, SLOT(updateObjectTransform()));

		eulerAxis2 = new QDoubleSpinBox(transformGroupBox);
		eulerAxis2->setGeometry(QRect(245, 65, 80, 22));
		SpinBoxStyleHelper::applyAngleStyleOn(eulerAxis2);
		connect(eulerAxis2, SIGNAL(valueChanged(double)), this, SLOT(updateObjectTransform()));
	}

	void ObjectControllerWidget::setupScale(QGroupBox *transformGroupBox)
	{
		QLabel *angleLabel = new QLabel(transformGroupBox);
		angleLabel->setText("Scale:");
		angleLabel->setGeometry(QRect(5, 90, 80, 22));

		scale = new QDoubleSpinBox(transformGroupBox);
		scale->setGeometry(QRect(85, 90, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(scale);
		scale->setMinimum(0.0);
		connect(scale, SIGNAL(valueChanged(double)), this, SLOT(updateObjectScale()));
	}

	void ObjectControllerWidget::setupFlagsBox(QWidget *tabGeneral)
	{
		QGroupBox *flagsGroupBox = new QGroupBox(tabGeneral);
		flagsGroupBox->setTitle("Flags");
		flagsGroupBox->setGeometry(QRect(0, 135, 350, 45));
		GroupBoxStyleHelper::applyNormalStyle(flagsGroupBox);

		produceShadowCheckBox = new QCheckBox(flagsGroupBox);
		produceShadowCheckBox->setText("Product Shadow");
		produceShadowCheckBox->setGeometry(QRect(5, 15, 110, 22));
		connect(produceShadowCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateObjectFlags()));
	}

	void ObjectControllerWidget::setupPhysicsBox(QWidget *tabPhysics)
	{
		hasRigidBody = new QCheckBox(tabPhysics);
		hasRigidBody->setText("Rigid Body");
		hasRigidBody->setGeometry(QRect(0, 10, 85, 15));
		connect(hasRigidBody, SIGNAL(stateChanged(int)), this, SLOT(rigidBodyToggled(int)));

		tabPhysicsRigidBody = new QTabWidget(tabPhysics);
		tabPhysicsRigidBody->setGeometry(QRect(0, 35, 350, 460));

		tabPhysicsProperties = new QWidget();
		setupPhysicsGeneralPropertiesBox();
		setupPhysicsDampingPropertiesBox();
		setupPhysicsFactorPropertiesBox();
		tabPhysicsRigidBody->addTab(tabPhysicsProperties, "Properties");

		tabPhysicsShape = new QWidget();
		setupPhysicsShapeBox();
		tabPhysicsRigidBody->addTab(tabPhysicsShape, "Shape");
	}

	void ObjectControllerWidget::setupPhysicsGeneralPropertiesBox()
	{
		QGroupBox *rigidBodyGeneralBox = new QGroupBox(tabPhysicsProperties);
		rigidBodyGeneralBox->setTitle("General");
		rigidBodyGeneralBox->setGeometry(QRect(0, 5, 345, 70));
		GroupBoxStyleHelper::applyNormalStyle(rigidBodyGeneralBox);

		QLabel *massLabel = new QLabel(rigidBodyGeneralBox);
		massLabel->setText("Mass:");
		massLabel->setGeometry(QRect(5, 15, 70, 22));

		mass = new QDoubleSpinBox(rigidBodyGeneralBox);
		mass->setGeometry(QRect(75, 15, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(mass);
		mass->setMinimum(0.0);
		connect(mass, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));

		QLabel *resitutionLabel = new QLabel(rigidBodyGeneralBox);
		resitutionLabel->setText("Restitution:");
		resitutionLabel->setGeometry(QRect(175, 15, 60, 22));

		restitution = new QDoubleSpinBox(rigidBodyGeneralBox);
		restitution->setGeometry(QRect(255, 15, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(restitution);
		restitution->setMinimum(0.0);
		restitution->setMaximum(1.0);
		connect(restitution, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));

		QLabel *frictionLabel = new QLabel(rigidBodyGeneralBox);
		frictionLabel->setText("Friction:");
		frictionLabel->setGeometry(QRect(5, 40, 70, 22));

		friction = new QDoubleSpinBox(rigidBodyGeneralBox);
		friction->setGeometry(QRect(75, 40, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(friction);
		friction->setMinimum(0.0);
		friction->setMaximum(1.0);
		connect(friction, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));

		QLabel *rollingFrictionLabel = new QLabel(rigidBodyGeneralBox);
		rollingFrictionLabel->setText("Rolling Friction:");
		rollingFrictionLabel->setGeometry(QRect(175, 40, 80, 22));

		rollingFriction = new QDoubleSpinBox(rigidBodyGeneralBox);
		rollingFriction->setGeometry(QRect(255, 40, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(rollingFriction);
		rollingFriction->setMinimum(0.0);
		rollingFriction->setMaximum(1.0);
		connect(rollingFriction, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));
	}

	void ObjectControllerWidget::setupPhysicsDampingPropertiesBox()
	{
		QGroupBox *rigidBodyDampingBox = new QGroupBox(tabPhysicsProperties);
		rigidBodyDampingBox->setTitle("Damping");
		rigidBodyDampingBox->setGeometry(QRect(0, 80, 345, 45));
		GroupBoxStyleHelper::applyNormalStyle(rigidBodyDampingBox);

		QLabel *linearDampingLabel = new QLabel(rigidBodyDampingBox);
		linearDampingLabel->setText("Linear:");
		linearDampingLabel->setGeometry(QRect(5, 15, 70, 22));

		linearDamping = new QDoubleSpinBox(rigidBodyDampingBox);
		linearDamping->setGeometry(QRect(75, 15, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(linearDamping);
		linearDamping->setMinimum(0.0);
		linearDamping->setMaximum(1.0);
		connect(linearDamping, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));

		QLabel *angularDampingLabel = new QLabel(rigidBodyDampingBox);
		angularDampingLabel->setText("Angular:");
		angularDampingLabel->setGeometry(QRect(175, 15, 60, 22));

		angularDamping = new QDoubleSpinBox(rigidBodyDampingBox);
		angularDamping->setGeometry(QRect(255, 15, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(angularDamping);
		angularDamping->setMinimum(0.0);
		angularDamping->setMaximum(1.0);
		connect(angularDamping, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));
	}

	void ObjectControllerWidget::setupPhysicsFactorPropertiesBox()
	{
		QGroupBox *rigidBodyFactorBox = new QGroupBox(tabPhysicsProperties);
		rigidBodyFactorBox->setTitle("Factor");
		rigidBodyFactorBox->setGeometry(QRect(0, 130, 345, 70));
		GroupBoxStyleHelper::applyNormalStyle(rigidBodyFactorBox);

		QLabel *linearFactorLabel = new QLabel(rigidBodyFactorBox);
		linearFactorLabel->setText("Linear:");
		linearFactorLabel->setGeometry(QRect(5, 15, 70, 22));

		linearFactorX = new QDoubleSpinBox(rigidBodyFactorBox);
		linearFactorX->setGeometry(QRect(75, 15, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(linearFactorX);
		linearFactorX->setMinimum(0.0);
		linearFactorX->setMaximum(1.0);
		connect(linearFactorX, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));

		linearFactorY = new QDoubleSpinBox(rigidBodyFactorBox);
		linearFactorY->setGeometry(QRect(155, 15, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(linearFactorY);
		linearFactorY->setMinimum(0.0);
		linearFactorY->setMaximum(1.0);
		connect(linearFactorY, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));

		linearFactorZ = new QDoubleSpinBox(rigidBodyFactorBox);
		linearFactorZ->setGeometry(QRect(235, 15, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(linearFactorZ);
		linearFactorZ->setMinimum(0.0);
		linearFactorZ->setMaximum(1.0);
		connect(linearFactorZ, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));

		QLabel *angularFactorLabel = new QLabel(rigidBodyFactorBox);
		angularFactorLabel->setText("Angular:");
		angularFactorLabel->setGeometry(QRect(5, 40, 70, 22));

		angularFactorX = new QDoubleSpinBox(rigidBodyFactorBox);
		angularFactorX->setGeometry(QRect(75, 40, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(angularFactorX);
		angularFactorX->setMinimum(0.0);
		angularFactorX->setMaximum(1.0);
		connect(angularFactorX, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));

		angularFactorY = new QDoubleSpinBox(rigidBodyFactorBox);
		angularFactorY->setGeometry(QRect(155, 40, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(angularFactorY);
		angularFactorY->setMinimum(0.0);
		angularFactorY->setMaximum(1.0);
		connect(angularFactorY, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));

		angularFactorZ = new QDoubleSpinBox(rigidBodyFactorBox);
		angularFactorZ->setGeometry(QRect(235, 40, 80, 22));
		SpinBoxStyleHelper::applyDefaultStyleOn(angularFactorZ);
		angularFactorZ->setMinimum(0.0);
		angularFactorZ->setMaximum(1.0);
		connect(angularFactorZ, SIGNAL(valueChanged(double)), this, SLOT(updateObjectPhysicsProperties()));
	}

	void ObjectControllerWidget::setupPhysicsShapeBox()
	{
		QLabel *shapeTypeLabel = new QLabel(tabPhysicsShape);
		shapeTypeLabel->setText("Shape Type:");
		shapeTypeLabel->setGeometry(QRect(5, 5, 80, 22));

		shapeTypeValueLabel = new QLabel(tabPhysicsShape);
		shapeTypeValueLabel->setGeometry(QRect(85, 5, 90, 22));

		changeBodyShapeButton = new QPushButton(tabPhysicsShape);
		changeBodyShapeButton->setText("Change Shape");
		changeBodyShapeButton->setGeometry(QRect(180, 5, 85, 22));
		connect(changeBodyShapeButton, SIGNAL(clicked()), this, SLOT(showChangeBodyShapeDialog()));

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
		bodyShapeWidget = BodyShapeWidgetRetriever(tabPhysicsShape, sceneObject).retrieveShapeWidget(shape);
		connect(bodyShapeWidget, SIGNAL(bodyShapeChange(std::shared_ptr<const CollisionShape3D>)), this, SLOT(bodyShapeChanged(std::shared_ptr<const CollisionShape3D>)));

		bodyShapeWidget->setGeometry(QRect(0, 30, 339, 375));
		bodyShapeWidget->show();

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
