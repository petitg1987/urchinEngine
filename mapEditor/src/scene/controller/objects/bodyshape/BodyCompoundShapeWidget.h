#ifndef URCHINENGINE_MAPEDITOR_BODYCOMPOUNDSHAPEWIDGET_H
#define URCHINENGINE_MAPEDITOR_BODYCOMPOUNDSHAPEWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGridLayout>

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
			explicit BodyCompoundShapeWidget(const SceneObject *);
			~BodyCompoundShapeWidget() override = default;;

			std::string getBodyShapeName() const override;
			LocalizedShapeTableView *getLocalizedShapeTableView() const;

		protected:
			void doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D>) override;
			std::shared_ptr<const CollisionShape3D> createBodyShape() const override;

		private:
			void notify(Observable *, int) override;

			void setupTransformBox(QVBoxLayout *, const std::shared_ptr<const LocalizedCollisionShape> &);
			void setupPosition(QGridLayout *, const Point3<float> &);
			void setupOrientation(QGridLayout *, const Quaternion<float> &);
			void setupShapeBox(QVBoxLayout *, const std::shared_ptr<const LocalizedCollisionShape> &);

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
