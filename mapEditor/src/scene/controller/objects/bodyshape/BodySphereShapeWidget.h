#ifndef ENGINE_MAPEDITOR_BODYSPHERESHAPEWIDGET_H
#define ENGINE_MAPEDITOR_BODYSPHERESHAPEWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QDoubleSpinBox>

#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"
#include "scene/controller/objects/bodyshape/BodyShapeWidget.h"

namespace urchin
{

	class BodySphereShapeWidget : public BodyShapeWidget
	{
		Q_OBJECT

		public:
			BodySphereShapeWidget(const SceneObject *);
			virtual ~BodySphereShapeWidget();

			std::string getBodyShapeName() const;

		protected:
			void doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D>);
			std::shared_ptr<const CollisionShape3D> createBodyShape() const;

		private:
			QDoubleSpinBox *radius;
	};

}

#endif
