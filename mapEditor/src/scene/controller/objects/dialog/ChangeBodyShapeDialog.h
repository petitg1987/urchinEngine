#ifndef ENGINE_MAPEDITOR_CHANGEBODYSHAPEDIALOG_H
#define ENGINE_MAPEDITOR_CHANGEBODYSHAPEDIALOG_H

#include <list>
#include <QtWidgets/QDialog>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>

#include "UrchinMapHandler.h"
#include "UrchinPhysicsEngine.h"

namespace urchin
{

	class ChangeBodyShapeDialog : public QDialog
	{
		Q_OBJECT

		public:
			ChangeBodyShapeDialog(QWidget *parent, bool);

			CollisionShape3D::ShapeType getShapeType() const;

		private:
			void setupBodyShapeTypeFields();

			void done(int);

			bool excludeCompoundShape;

			QLabel *bodyShapeTypeLabel;
			QComboBox *bodyShapeTypeComboBox;

			CollisionShape3D::ShapeType shapeType;
	};

}

#endif
