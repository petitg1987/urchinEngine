#ifndef URCHINENGINE_MAPEDITOR_BODYCAPSULESHAPEWIDGET_H
#define URCHINENGINE_MAPEDITOR_BODYCAPSULESHAPEWIDGET_H

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
			explicit BodyCapsuleShapeWidget(const SceneObject *);
			~BodyCapsuleShapeWidget() override = default;

			std::string getBodyShapeName() const override;

		protected:
			void doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D>) override;
			std::shared_ptr<const CollisionShape3D> createBodyShape() const override;

		private:
			QDoubleSpinBox *radius, *cylinderHeight;
			QComboBox *orientation;
	};

}

#endif
