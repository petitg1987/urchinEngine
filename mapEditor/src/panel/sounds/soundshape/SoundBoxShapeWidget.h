#ifndef URCHINENGINE_SOUNDBOXSHAPEWIDGET_H
#define URCHINENGINE_SOUNDBOXSHAPEWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QComboBox>

#include "UrchinCommon.h"
#include "UrchinSoundEngine.h"
#include "panel/sounds/soundshape/SoundShapeWidget.h"

namespace urchin
{

	class SoundBoxShapeWidget : public SoundShapeWidget
	{
		Q_OBJECT

		public:
			explicit SoundBoxShapeWidget(const SceneSound *);
			~SoundBoxShapeWidget() override = default;

			std::string getSoundShapeName() const override;

		protected:
			void doSetupShapePropertiesFrom(const SoundShape *) override;
			const SoundShape *createSoundShape() const override;

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
