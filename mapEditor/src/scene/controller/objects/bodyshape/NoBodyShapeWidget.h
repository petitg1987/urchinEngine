#ifndef URCHINENGINE_MAPEDITOR_NOBODYSHAPEWIDGET_H
#define URCHINENGINE_MAPEDITOR_NOBODYSHAPEWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QDoubleSpinBox>

#include "UrchinPhysicsEngine.h"
#include "scene/controller/objects/bodyshape/BodyShapeWidget.h"

namespace urchin
{

	class NoBodyShapeWidget : public BodyShapeWidget
	{
		Q_OBJECT

		public:
			NoBodyShapeWidget(const SceneObject *);
			virtual ~NoBodyShapeWidget();

			std::string getBodyShapeName() const;

		protected:
			void doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D>);
			std::shared_ptr<const CollisionShape3D> createBodyShape() const;
	};

}

#endif
