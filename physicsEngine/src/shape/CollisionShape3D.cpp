#include "shape/CollisionShape3D.h"

#define DEFAULT_INNER_MARGIN 0.04

namespace urchin
{

	CollisionShape3D::CollisionShape3D() :
			innerMargin(DEFAULT_INNER_MARGIN)
	{

	}

	CollisionShape3D::CollisionShape3D(float innerMargin) :
			innerMargin(innerMargin)
	{

	}

	CollisionShape3D::~CollisionShape3D()
	{

	}

	/**
	 * @param innerMargin This value is to avoid doing costly penetration depth calculation.
	 * - if define too small, the performance degrades
	 * - if define too big, the objects will be too rounded
	 */
	void CollisionShape3D::setInnerMargin(float innerMargin)
	{
		this->innerMargin = innerMargin;
	}

	float CollisionShape3D::getInnerMargin() const
	{
		return innerMargin;
	}

}
