#ifndef ENGINE_MAPEDITOR_SOUNDBOXSHAPEWIDGET_H
#define ENGINE_MAPEDITOR_SOUNDBOXSHAPEWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QDoubleSpinBox>

#include "UrchinCommon.h"
#include "UrchinSoundEngine.h"
#include "scene/controller/sounds/soundshape/SoundShapeWidget.h"

namespace urchin
{

	class SoundBoxShapeWidget : public SoundShapeWidget
	{
		Q_OBJECT

		public:
			SoundBoxShapeWidget(QWidget *, const SceneSound *);
			virtual ~SoundBoxShapeWidget();

			std::string getSoundShapeName() const;

		protected:
			void doSetupShapePropertiesFrom(const SoundShape *);
			const SoundShape *createSoundShape() const;

		private:
			QDoubleSpinBox *positionX, *positionY, *positionZ;
			QDoubleSpinBox *halfSizeX, *halfSizeY, *halfSizeZ;
			QDoubleSpinBox *orientationAxisX, *orientationAxisY, *orientationAxisZ, *orientationAngle;
			QDoubleSpinBox *margin;
	};

}

#endif
