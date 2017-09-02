#ifndef URCHINENGINE_MAPEDITOR_OBJECTCONTROLLERWIDGET_H
#define URCHINENGINE_MAPEDITOR_OBJECTCONTROLLERWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QTabWidget>

#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"
#include "scene/controller/objects/ObjectController.h"
#include "scene/controller/objects/ObjectTableView.h"
#include "scene/controller/objects/bodyshape/BodyShapeWidget.h"

namespace urchin
{

	class ObjectControllerWidget : public QWidget, public Observer, public Observable
	{
		Q_OBJECT

		public:
			ObjectControllerWidget();
			virtual ~ObjectControllerWidget() = default;

			enum NotificationType
			{
				BODY_SHAPE_INITIALIZED
			};

			ObjectTableView *getObjectTableView() const;
			BodyShapeWidget *getBodyShapeWidget() const;

			void load(ObjectController *);
			void unload();

		private:
			void setupTransformBox(QVBoxLayout *);
			void setupPosition(QGridLayout *);
			void setupOrientation(QGridLayout *);
			void setupScale(QGridLayout *);
			void setupFlagsBox(QVBoxLayout *);
			void setupPhysicsBox(QVBoxLayout *);
			void setupPhysicsGeneralPropertiesBox(QVBoxLayout *);
			void setupPhysicsDampingPropertiesBox(QVBoxLayout *);
			void setupPhysicsFactorPropertiesBox(QVBoxLayout *);
			void setupPhysicsShapeBox(QVBoxLayout *);

			void notify(Observable *, int);
			void setupObjectDataFrom(const SceneObject *);
			void setupObjectPhysicsDataFrom(const SceneObject *);

			BodyShapeWidget *retrieveBodyShapeWidget(std::shared_ptr<const CollisionShape3D>, const SceneObject *);

			ObjectController *objectController;

			ObjectTableView *objectTableView;
			QPushButton *addObjectButton;
			QPushButton *removeObjectButton;
			QTabWidget *tabWidget;

			bool disableObjectEvent;

			//general
			QDoubleSpinBox *positionX, *positionY, *positionZ;
			QComboBox *orientationType;
			QDoubleSpinBox *eulerAxis0, *eulerAxis1, *eulerAxis2;
			QDoubleSpinBox *scale;
			QCheckBox *produceShadowCheckBox;

			//physics
			QCheckBox *hasRigidBody;
			QTabWidget *tabPhysicsRigidBody;
			QVBoxLayout *physicsShapeLayout;
			QDoubleSpinBox *mass, *restitution, *friction, *rollingFriction;
			QDoubleSpinBox *linearDamping, *angularDamping;
			QDoubleSpinBox *linearFactorX, *linearFactorY, *linearFactorZ;
			QDoubleSpinBox *angularFactorX, *angularFactorY, *angularFactorZ;
			QLabel *shapeTypeValueLabel;
			QPushButton *changeBodyShapeButton;
			BodyShapeWidget *bodyShapeWidget;

		private slots:
			void showAddObjectDialog();
			void removeSelectedObject();

			void updateObjectOrientationType();
			void updateObjectTransform();
			void updateObjectScale();
			void updateObjectFlags();

			void showChangeBodyShapeDialog();
			void rigidBodyToggled(int);
			void updateObjectPhysicsProperties();
			void bodyShapeChanged(std::shared_ptr<const CollisionShape3D>);
	};

}

#endif
