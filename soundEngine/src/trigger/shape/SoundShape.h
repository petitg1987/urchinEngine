#ifndef ENGINE_SOUNDSHAPE_H
#define ENGINE_SOUNDSHAPE_H

#include "UrchinCommon.h"

namespace urchin
{

	/**
	* Shape used to delimit the sound
	*/
	class SoundShape
	{
		public:
			enum ShapeType
			{
				SPHERE_SHAPE,
				BOX_SHAPE
			};

			SoundShape(float);
			virtual ~SoundShape();

			float getMargin() const;

			virtual ShapeType getShapeType() const = 0;

			virtual bool pointInsidePlayShape(const Point3<float> &) const = 0;
			virtual bool pointInsideStopShape(const Point3<float> &) const = 0;

			virtual SoundShape *clone() const = 0;

		private:
			const float margin;
	};

}

#endif
