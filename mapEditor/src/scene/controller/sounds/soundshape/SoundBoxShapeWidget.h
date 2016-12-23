#ifndef ENGINE_MAPEDITOR_SOUNDBOXSHAPEWIDGET_H
#define ENGINE_MAPEDITOR_SOUNDBOXSHAPEWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QComboBox>

#include "UrchinCommon.h"
#include "UrchinSoundEngine.h"
#include "scene/controller/sounds/soundshape/SoundShapeWidget.h"

namespace urchin
{

	class SoundBoxShapeWidget : public SoundShapeWidget
	{
		Q_OBJECT

		public:
			SoundBoxShapeWidget(const SceneSound *);
			virtual ~SoundBoxShapeWidget();

			std::string getSoundShapeName() const;

		protected:
			void doSetupShapePropertiesFrom(const SoundShape *);
			const SoundShape *createSoundShape() const;

		private:
			QDoubleSpinBox *positionX, *positionY, *positionZ;
			QDoubleSpinBox *halfSizeX, *halfSizeY, *halfSizeZ;
			QComboBox *orientationType;
			QDoubleSpinBox *eulerAxis0, *eulerAxis1, *eulerAxis2;

		private slots:
			void updateShapeOrientationType();
	};

}

#endif
