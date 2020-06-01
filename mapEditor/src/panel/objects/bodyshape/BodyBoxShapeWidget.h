#ifndef URCHINENGINE_MAPEDITOR_BODYBOXSHAPEWIDGET_H
#define URCHINENGINE_MAPEDITOR_BODYBOXSHAPEWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QDoubleSpinBox>

#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"
#include "panel/objects/bodyshape/BodyShapeWidget.h"

namespace urchin
{

	class BodyBoxShapeWidget : public BodyShapeWidget
	{
		Q_OBJECT

		public:
			explicit BodyBoxShapeWidget(const SceneObject *);
			~BodyBoxShapeWidget() override = default;

			std::string getBodyShapeName() const override;

		protected:
			void doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D>) override;
			std::shared_ptr<const CollisionShape3D> createBodyShape() const override;

		private:
			QDoubleSpinBox *halfSizeX, *halfSizeY, *halfSizeZ;
	};

}

#endif
