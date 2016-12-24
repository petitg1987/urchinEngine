#ifndef ENGINE_MAPEDITOR_BODYCAPSULESHAPEWIDGET_H
#define ENGINE_MAPEDITOR_BODYCAPSULESHAPEWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QComboBox>

#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"
#include "scene/controller/objects/bodyshape/BodyShapeWidget.h"

namespace urchin
{

	class BodyCapsuleShapeWidget : public BodyShapeWidget
	{
		Q_OBJECT

		public:
			BodyCapsuleShapeWidget(const SceneObject *);
			virtual ~BodyCapsuleShapeWidget();

			std::string getBodyShapeName() const;

		protected:
			void doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D>);
			std::shared_ptr<const CollisionShape3D> createBodyShape() const;

		private:
			QDoubleSpinBox *radius, *cylinderHeight;
			QComboBox *orientation;
	};

}

#endif
