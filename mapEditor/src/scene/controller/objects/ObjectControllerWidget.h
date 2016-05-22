#ifndef ENGINE_MAPEDITOR_OBJECTCONTROLLERWIDGET_H
#define ENGINE_MAPEDITOR_OBJECTCONTROLLERWIDGET_H

#include <string>
#include <memory>
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

		#define EULER_XYZ_ORIENT_LABEL "Euler XYZ"
		#define EULER_XZY_ORIENT_LABEL "Euler XZY"
		#define EULER_YXZ_ORIENT_LABEL "Euler YXZ"
		#define EULER_YZX_ORIENT_LABEL "Euler YZX"
		#define EULER_ZXY_ORIENT_LABEL "Euler ZXY"
		#define EULER_ZYX_ORIENT_LABEL "Euler ZYX"
		#define EULER_XYX_ORIENT_LABEL "Euler XYX"
		#define EULER_XZX_ORIENT_LABEL "Euler XZX"
		#define EULER_YXY_ORIENT_LABEL "Euler YXY"
		#define EULER_YZY_ORIENT_LABEL "Euler YZY"
		#define EULER_ZXZ_ORIENT_LABEL "Euler ZXZ"
		#define EULER_ZYZ_ORIENT_LABEL "Euler ZYZ"

		public:
			ObjectControllerWidget();
			virtual ~ObjectControllerWidget();

			enum NotificationType
			{
				BODY_SHAPE_INITIALIZED
			};

			ObjectTableView *getObjectTableView() const;
			BodyShapeWidget *getBodyShapeWidget() const;

			void load(ObjectController *);
			void unload();

			bool isModified() const;

		private:
			void setupTransformBox(QWidget *);
			void setupPosition(QGroupBox *);
			void setupOrientation(QGroupBox *);
			void setupScale(QGroupBox *);
			void setupFlagsBox(QWidget *);
			void setupPhysicsBox(QWidget *);
			void setupPhysicsGeneralPropertiesBox();
			void setupPhysicsDampingPropertiesBox();
			void setupPhysicsFactorPropertiesBox();
			void setupPhysicsShapeBox();

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
			QWidget *tabPhysicsProperties, *tabPhysicsShape;
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
