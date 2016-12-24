#ifndef ENGINE_MAPEDITOR_BODYCYLINDERSHAPEWIDGET_H
#define ENGINE_MAPEDITOR_BODYCYLINDERSHAPEWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QComboBox>

#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"
#include "scene/controller/objects/bodyshape/BodyShapeWidget.h"

namespace urchin
{

	class BodyCylinderShapeWidget : public BodyShapeWidget
	{
		Q_OBJECT

		public:
			BodyCylinderShapeWidget(const SceneObject *);
			virtual ~BodyCylinderShapeWidget();

			std::string getBodyShapeName() const;

		protected:
			void doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D>);
			std::shared_ptr<const CollisionShape3D> createBodyShape() const;

		private:
			QDoubleSpinBox *radius, *height;
			QComboBox *orientation;
	};

}

#endif
