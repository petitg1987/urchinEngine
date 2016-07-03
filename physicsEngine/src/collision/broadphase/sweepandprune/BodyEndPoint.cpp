#include "collision/broadphase/sweepandprune/BodyEndPoint.h"

namespace urchin
{

	BodyEndPoint::BodyEndPoint() :
		bodyBox(nullptr),
		bIsMin(false),
		value(0)
	{

	}

	BodyEndPoint::~BodyEndPoint()
	{

	}

	void BodyEndPoint::setBodyBox(BodyBox *bodyBox)
	{
		this->bodyBox = bodyBox;
	}

	BodyBox *BodyEndPoint::getBodyBox() const
	{
		return bodyBox;
	}

	/**
	 * @param isMin Minimum/maximum flag
	 */
	void BodyEndPoint::setIsMin(bool isMin)
	{
		this->bIsMin = isMin;
	}

	/**
	 * @return Minimum/maximum flag
	 */
	bool BodyEndPoint::isMin() const
	{
		return bIsMin;
	}

	void BodyEndPoint::setValue(float value)
	{
		this->value = value;
	}

	float BodyEndPoint::getValue() const
	{
		return value;
	}

}
