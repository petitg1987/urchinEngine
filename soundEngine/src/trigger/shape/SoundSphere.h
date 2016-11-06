#ifndef ENGINE_SOUNDSPHERE_H
#define ENGINE_SOUNDSPHERE_H

#include "UrchinCommon.h"

#include "trigger/shape/SoundShape.h"

namespace urchin
{

	/**
	* Sphere used to delimit the sound
	*/
	class SoundSphere : public SoundShape
	{
		public:
			SoundSphere(float, const Point3<float> &, float);
			~SoundSphere();

			SoundShape::ShapeType getShapeType() const;

			const Sphere<float> &getPlayTriggerSphere() const;
			const Sphere<float> &getStopTriggerSphere() const;

			float getRadius() const;
			const Point3<float> getPosition() const;

			bool pointInsidePlayShape(const Point3<float> &) const;
			bool pointInsideStopShape(const Point3<float> &) const;

			SoundShape *clone() const;

		private:
			const Sphere<float> playTriggerSphere;
			const Sphere<float> stopTriggerSphere;
	};

}

#endif
