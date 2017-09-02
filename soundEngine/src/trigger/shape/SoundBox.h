#ifndef URCHINENGINE_SOUNDBOX_H
#define URCHINENGINE_SOUNDBOX_H

#include "UrchinCommon.h"

#include "trigger/shape/SoundShape.h"

namespace urchin
{

	/**
	* Box used to delimit the sound
	*/
	class SoundBox : public SoundShape
	{
		public:
			SoundBox(const Vector3<float> &, const Point3<float> &, const Quaternion<float> &, float);

			SoundShape::ShapeType getShapeType() const override;

			const OBBox<float> &getPlayTriggerBox() const;
			const OBBox<float> &getStopTriggerBox() const;

			const Vector3<float> &getHalfSizes() const;
			const Point3<float> &getCenterPosition() const;
			const Quaternion<float> &getOrientation() const;
			const Vector3<float> &getAxis(unsigned int) const;

			bool pointInsidePlayShape(const Point3<float> &) const override;
			bool pointInsideStopShape(const Point3<float> &) const override;

			SoundShape *clone() const override;

		private:
			bool pointInsideShape(const Point3<float> &, const OBBox<float> &) const;

			const OBBox<float> playTriggerBox;
			const OBBox<float> stopTriggerBox;
	};

}

#endif
