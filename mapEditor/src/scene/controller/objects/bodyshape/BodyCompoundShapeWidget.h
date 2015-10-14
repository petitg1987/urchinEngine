#ifndef ENGINE_MAPEDITOR_BODYCOMPOUNDSHAPEWIDGET_H
#define ENGINE_MAPEDITOR_BODYCOMPOUNDSHAPEWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDoubleSpinBox>

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

			QLabel *shapesLabel;
			LocalizedShapeTableView *localizedShapeTableView;
			QPushButton *addShapeButton, *removeShapeButton;

			QWidget *localizedShapeDetails;
			QDoubleSpinBox *translationX, *translationY, *translationZ;
			BodyShapeWidget *bodyShapeWidget;

		private slots:
			void addNewLocalizedShape();
			void removeSelectedLocalizedShape();
			void updateSelectedLocalizedShape();

	};

}

#endif
