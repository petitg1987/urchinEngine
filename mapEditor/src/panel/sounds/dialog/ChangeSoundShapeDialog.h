#ifndef URCHINENGINE_CHANGESOUNDSHAPEDIALOG_H
#define URCHINENGINE_CHANGESOUNDSHAPEDIALOG_H

#include <list>
#include <QtWidgets/QGridLayout>
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
			explicit ChangeSoundShapeDialog(QWidget *parent);

			SoundShape::ShapeType getShapeType() const;

		private:
			void setupSoundShapeTypeFields(QGridLayout *);

			void done(int) override;

			QLabel *soundShapeTypeLabel;
			QComboBox *soundShapeTypeComboBox;

			SoundShape::ShapeType shapeType;
	};

}

#endif
