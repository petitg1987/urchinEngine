#ifndef ENGINE_MAPEDITOR_SOUNDSPHERESHAPEWIDGET_H
#define ENGINE_MAPEDITOR_SOUNDSPHERESHAPEWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QDoubleSpinBox>

#include "UrchinCommon.h"
#include "UrchinSoundEngine.h"
#include "scene/controller/sounds/soundshape/SoundShapeWidget.h"

namespace urchin
{

	class SoundSphereShapeWidget : public SoundShapeWidget
	{
		Q_OBJECT

		public:
			SoundSphereShapeWidget(const SceneSound *);
			virtual ~SoundSphereShapeWidget();

			std::string getSoundShapeName() const;

		protected:
			void doSetupShapePropertiesFrom(const SoundShape *);
			const SoundShape *createSoundShape() const;

		private:
			QDoubleSpinBox *radius;
			QDoubleSpinBox *positionX, *positionY, *positionZ;
	};

}

#endif
