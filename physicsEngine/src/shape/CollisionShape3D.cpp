#include "shape/CollisionShape3D.h"

namespace urchin
{

	CollisionShape3D::CollisionShape3D() :
			innerMargin(ConfigService::instance()->getFloatValue("collisionShape.defaultValue.margin"))
	{

	}

	CollisionShape3D::CollisionShape3D(float innerMargin) :
			innerMargin(innerMargin)
	{

	}

	CollisionShape3D::~CollisionShape3D()
	{

	}

	void CollisionShape3D::setInnerMargin(float innerMargin)
	{
		this->innerMargin = innerMargin;
	}

	float CollisionShape3D::getInnerMargin() const
	{
		return innerMargin;
	}

}
