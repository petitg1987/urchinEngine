#include <sstream>

#include "shape/CollisionShape3D.h"

namespace urchin
{

	//static
	std::set<CollisionShape3D::ShapeType> CollisionShape3D::CONVEX_SHAPES = {CollisionShape3D::SPHERE_SHAPE, CollisionShape3D::BOX_SHAPE,
                                                                             CollisionShape3D::CAPSULE_SHAPE, CollisionShape3D::CYLINDER_SHAPE,
                                                                             CollisionShape3D::CONE_SHAPE, CollisionShape3D::CONVEX_HULL_SHAPE,
																			 CollisionShape3D::TRIANGLE_SHAPE};
	std::set<CollisionShape3D::ShapeType> CollisionShape3D::CONCAVE_SHAPES = {CollisionShape3D::HEIGHTFIELD_SHAPE};
	std::set<CollisionShape3D::ShapeType> CollisionShape3D::COMPOUND_SHAPES = {CollisionShape3D::COMPOUND_SHAPE};

	CollisionShape3D::CollisionShape3D() :
			innerMargin(ConfigService::instance()->getFloatValue("collisionShape.innerMargin")),
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
			constexpr float RELATIVE_MARGIN_FACTOR_BIG_SHAPE = 200.0;
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

	bool CollisionShape3D::isConvex() const
	{
		return CONVEX_SHAPES.find(getShapeType()) != CONVEX_SHAPES.end();
	}

	bool CollisionShape3D::isConcave() const
	{
		return CONCAVE_SHAPES.find(getShapeType()) != CONCAVE_SHAPES.end();
	}

	bool CollisionShape3D::isCompound() const
	{
		return COMPOUND_SHAPES.find(getShapeType()) != COMPOUND_SHAPES.end();
	}
}
