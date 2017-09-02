#include <sstream>

#include "shape/CollisionShape3D.h"

namespace urchin
{

	CollisionShape3D::CollisionShape3D() :
			innerMargin(ConfigService::instance()->getUnsignedIntValue("collisionShape.innerMargin")),
			initialInnerMargin(innerMargin)
	{

	}

	CollisionShape3D::CollisionShape3D(float innerMargin) :
			innerMargin(innerMargin),
			initialInnerMargin(innerMargin)
	{

	}

	void CollisionShape3D::refreshInnerMargin(float maximumInnerMargin)
	{
		if(this->innerMargin > maximumInnerMargin)
		{
			this->innerMargin = maximumInnerMargin;
		}
	}

	float CollisionShape3D::getInnerMargin() const
	{
		return innerMargin;
	}

	void CollisionShape3D::checkInnerMarginQuality(const std::string &shapeId) const
	{
		if(initialInnerMargin > innerMargin)
		{
			constexpr float RELATIVE_MARGIN_FACTOR_BIG_SHAPE = 20.0;
			AABBox<float> aabbox = toAABBox(PhysicsTransform());
			float shapeLength = aabbox.getMin().vector(aabbox.getMax()).length();
			bool isBigShape = shapeLength > initialInnerMargin * RELATIVE_MARGIN_FACTOR_BIG_SHAPE;

			if(isBigShape)
			{
				std::stringstream logStream;
				logStream<<"Not optimal margin on shape id "<<shapeId<<"."<<std::endl;
				logStream<<" - Current margin: "<<innerMargin<<std::endl;
				logStream<<" - Expected margin: "<<initialInnerMargin<<std::endl;
				logStream<<" - Shape length: "<<shapeLength;
				Logger::logger().logWarning(logStream.str());
			}
		}
	}
}
