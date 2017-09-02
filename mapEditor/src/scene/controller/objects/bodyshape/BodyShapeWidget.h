#ifndef URCHINENGINE_MAPEDITOR_BODYSHAPEWIDGET_H
#define URCHINENGINE_MAPEDITOR_BODYSHAPEWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGridLayout>

#include "UrchinCommon.h"
#include "UrchinMapHandler.h"
#include "UrchinPhysicsEngine.h"

namespace urchin
{

	class BodyShapeWidget : public QWidget
	{
		Q_OBJECT

		#define SPHERE_SHAPE_LABEL "Sphere"
		#define BOX_SHAPE_LABEL "Box"
		#define CAPSULE_SHAPE_LABEL "Capsule"
		#define CYLINDER_SHAPE_LABEL "Cylinder"
		#define CONE_SHAPE_LABEL "Cone"
		#define CONVEX_HULL_SHAPE_LABEL "Convex Hull"
		#define COMPOUND_SHAPE_LABEL "Compound Shape"

		public:
			BodyShapeWidget(const SceneObject *);
			virtual ~BodyShapeWidget();

			const SceneObject *getSceneObject() const;
			virtual std::string getBodyShapeName() const = 0;
			std::shared_ptr<const CollisionShape3D> retrieveShape();

			void setupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D>);

		signals:
			void bodyShapeChange(std::shared_ptr<const CollisionShape3D>);

		public slots:
			void updateBodyShape();

		protected:
			QGridLayout *mainLayout;
			bool disableShapeEvent;

			virtual void doSetupShapePropertiesFrom(std::shared_ptr<const CollisionShape3D>) = 0;
			virtual std::shared_ptr<const CollisionShape3D> createBodyShape() const = 0;

		private:
			const SceneObject *sceneObject;

			std::shared_ptr<const CollisionShape3D> shape;
	};

}

#endif
