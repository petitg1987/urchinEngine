#ifndef ENGINE_MAPEDITOR_BODYBOXSHAPEWIDGET_H
#define ENGINE_MAPEDITOR_BODYBOXSHAPEWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QDoubleSpinBox>

#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"
#include "scene/controller/objects/bodyshape/BodyShapeWidget.h"

namespace urchin
{

	class BodyBoxShapeWidget : public BodyShapeWidget
	{
		Q_OBJECT

		public:
			BodyBoxShapeWidget(QWidget *, const SceneObject *);
			virtual ~BodyBoxShapeWidget();

			std::string getBodyShapeName() const;

		protected:
			void doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D>);
			std::shared_ptr<const CollisionShape3D> createBodyShape() const;

		private:
			QDoubleSpinBox *halfSizeX, *halfSizeY, *halfSizeZ;
	};

}

#endif
