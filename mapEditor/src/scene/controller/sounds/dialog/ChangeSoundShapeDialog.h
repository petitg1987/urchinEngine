#ifndef ENGINE_MAPEDITOR_CHANGESOUNDSHAPEDIALOG_H
#define ENGINE_MAPEDITOR_CHANGESOUNDSHAPEDIALOG_H

#include <list>
#include <QtWidgets/QDialog>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>

#include "UrchinMapHandler.h"
#include "UrchinSoundEngine.h"

namespace urchin
{

	class ChangeSoundShapeDialog : public QDialog
	{
		Q_OBJECT

		public:
			ChangeSoundShapeDialog(QWidget *parent);

			SoundShape::ShapeType getShapeType() const;

		private:
			void setupSoundShapeTypeFields();

			void done(int);

			QLabel *soundShapeTypeLabel;
			QComboBox *soundShapeTypeComboBox;

			SoundShape::ShapeType shapeType;
	};

}

#endif
