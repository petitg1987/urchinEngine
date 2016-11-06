#ifndef ENGINE_MAPEDITOR_BODYCOMPOUNDSHAPEWIDGET_H
#define ENGINE_MAPEDITOR_BODYCOMPOUNDSHAPEWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QComboBox>

#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"
#include "scene/controller/objects/bodyshape/BodyShapeWidget.h"
#include "scene/controller/objects/bodyshape/support/LocalizedShapeTableView.h"

namespace urchin
{

	class BodyCompoundShapeWidget : public BodyShapeWidget, public Observer
	{
		Q_OBJECT

		public:
			BodyCompoundShapeWidget(QWidget *, const SceneObject *);
			virtual ~BodyCompoundShapeWidget();

			std::string getBodyShapeName() const;
			LocalizedShapeTableView *getLocalizedShapeTableView() const;

		protected:
			void doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D>);
			std::shared_ptr<const CollisionShape3D> createBodyShape() const;

		private:
			void notify(Observable *, int);

			void setupTransformBox(QWidget *, std::shared_ptr<const LocalizedCollisionShape>);
			void setupPosition(QGroupBox *, const Point3<float> &);
			void setupOrientation(QGroupBox *, const Quaternion<float> &);
			void setupShapeBox(QWidget *, std::shared_ptr<const LocalizedCollisionShape>);

			QLabel *shapesLabel;
			LocalizedShapeTableView *localizedShapeTableView;
			QPushButton *addShapeButton, *removeShapeButton;

			QWidget *localizedShapeDetails;
			QDoubleSpinBox *positionX, *positionY, *positionZ;
			QComboBox *orientationType;
			QDoubleSpinBox *eulerAxis0, *eulerAxis1, *eulerAxis2;
			BodyShapeWidget *bodyShapeWidget;

		private slots:
			void addNewLocalizedShape();
			void removeSelectedLocalizedShape();
			void updateLocalizedShapeOrientationType();
			void updateSelectedLocalizedShape();

	};

}

#endif
