#ifndef ENGINE_MAPEDITOR_BODYCONESHAPEWIDGET_H
#define ENGINE_MAPEDITOR_BODYCONESHAPEWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QComboBox>

#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"
#include "scene/controller/objects/bodyshape/BodyShapeWidget.h"

namespace urchin
{

	class BodyConeShapeWidget : public BodyShapeWidget
	{
		Q_OBJECT

		public:
			BodyConeShapeWidget(const SceneObject *);
			virtual ~BodyConeShapeWidget();

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
