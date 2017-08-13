#include "trigger/shape/SoundBox.h"

namespace urchin
{

	/**
	 * @param margin Margin to box used to determine stop sound trigger
	 */
	SoundBox::SoundBox(const Vector3<float> &halfSizes, const Point3<float> &centerPosition, const Quaternion<float> &orientation, float margin) :
		SoundShape(margin),
		playTriggerBox(OBBox<float>(halfSizes, centerPosition, orientation)),
		stopTriggerBox(OBBox<float>(halfSizes + Vector3<float>(margin, margin, margin), centerPosition, orientation))
	{

	}

	SoundShape::ShapeType SoundBox::getShapeType() const
	{
		return SoundShape::BOX_SHAPE;
	}

	const OBBox<float> &SoundBox::getPlayTriggerBox() const
	{
		return playTriggerBox;
	}

	const OBBox<float> &SoundBox::getStopTriggerBox() const
	{
		return stopTriggerBox;
	}

	const Vector3<float> &SoundBox::getHalfSizes() const
	{
		return playTriggerBox.getHalfSizes();
	}

	const Point3<float> &SoundBox::getCenterPosition() const
	{
		return playTriggerBox.getCenterOfMass();
	}

	const Quaternion<float> &SoundBox::getOrientation() const
	{
		return playTriggerBox.getOrientation();
	}

	/**
	 * @return Bounding box normalized axis
	 */
	const Vector3<float> &SoundBox::getAxis(unsigned int index) const
	{
		return playTriggerBox.getAxis(index);
	}

	bool SoundBox::pointInsidePlayShape(const Point3<float> &point) const
	{
		return pointInsideShape(point, playTriggerBox);
	}

	bool SoundBox::pointInsideStopShape(const Point3<float> &point) const
	{
		return pointInsideShape(point, stopTriggerBox);
	}

	bool SoundBox::pointInsideShape(const Point3<float> &point, const OBBox<float> &box) const
	{
		Vector3<float> localPointTranslation = box.getCenterOfMass().vector(point);

		return std::fabs(localPointTranslation.dotProduct(box.getAxis(0))) <= box.getHalfSize(0) &&
				std::fabs(localPointTranslation.dotProduct(box.getAxis(1))) <= box.getHalfSize(1) &&
				std::fabs(localPointTranslation.dotProduct(box.getAxis(2))) <= box.getHalfSize(2);
	}

	SoundShape *SoundBox::clone() const
	{
		return new SoundBox(getHalfSizes(), getCenterPosition(), getOrientation(), getMargin());
	}

}
