#ifndef ENGINE_MAPEDITOR_SOUNDSHAPEWIDGET_H
#define ENGINE_MAPEDITOR_SOUNDSHAPEWIDGET_H

#include <string>
#include <memory>
#include <QtWidgets/QDoubleSpinBox>

#include "UrchinCommon.h"
#include "UrchinMapHandler.h"
#include "UrchinSoundEngine.h"

namespace urchin
{

	class SoundShapeWidget : public QWidget
	{
		Q_OBJECT

		#define SPHERE_SHAPE_LABEL "Sphere"
		#define BOX_SHAPE_LABEL "Box"

		public:
			SoundShapeWidget(QWidget *, const SceneSound *);
			virtual ~SoundShapeWidget();

			const SceneSound *getSceneSound() const;
			virtual std::string getSoundShapeName() const = 0;
			const SoundShape *retrieveShape();

			void setupShapePropertiesFrom(const SoundShape *);

		signals:
			void soundShapeChange(const SoundShape *);

		public slots:
			void updateSoundShape();

		protected:
			bool disableShapeEvent;

			float getMarginValue() const;

			virtual void doSetupShapePropertiesFrom(const SoundShape *) = 0;
			virtual const SoundShape *createSoundShape() const = 0;

		private:
			QDoubleSpinBox *margin;

			const SceneSound *sceneSound;
			const SoundShape *shape;
	};

}

#endif
