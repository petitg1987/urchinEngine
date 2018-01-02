#ifndef URCHINENGINE_MAPEDITOR_BODYCYLINDERSHAPEWIDGET_H
#define URCHINENGINE_MAPEDITOR_BODYCYLINDERSHAPEWIDGET_H

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
			explicit BodyCylinderShapeWidget(const SceneObject *);
			~BodyCylinderShapeWidget() override = default;

			std::string getBodyShapeName() const override;

		protected:
			void doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D>) override;
			std::shared_ptr<const CollisionShape3D> createBodyShape() const override;

		private:
			QDoubleSpinBox *radius, *height;
			QComboBox *orientation;
	};

}

#endif
