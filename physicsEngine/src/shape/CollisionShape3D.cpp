#include <sstream>
#include <algorithm>

#include "shape/CollisionShape3D.h"
#include "utils/property/EagerPropertyLoader.h"

namespace urchin
{

	//static
	std::vector<CollisionShape3D::ShapeType> CollisionShape3D::CONVEX_SHAPES = {CollisionShape3D::TRIANGLE_SHAPE, CollisionShape3D::SPHERE_SHAPE,
                                                                             CollisionShape3D::CAPSULE_SHAPE, CollisionShape3D::CYLINDER_SHAPE,
																			 CollisionShape3D::BOX_SHAPE, CollisionShape3D::CONVEX_HULL_SHAPE,
																			 CollisionShape3D::CONE_SHAPE};
	std::vector<CollisionShape3D::ShapeType> CollisionShape3D::CONCAVE_SHAPES = {CollisionShape3D::HEIGHTFIELD_SHAPE};
	std::vector<CollisionShape3D::ShapeType> CollisionShape3D::COMPOUND_SHAPES = {CollisionShape3D::COMPOUND_SHAPE};
	std::vector<CollisionShape3D::ShapeType> CollisionShape3D::SPHERE_SHAPES = {CollisionShape3D::SPHERE_SHAPE};

	CollisionShape3D::CollisionShape3D() :
			innerMargin(EagerPropertyLoader::instance()->getCollisionShapeInnerMargin()),
			initialInnerMargin(innerMargin)
	{
		lastTransform.setPosition(Point3<float>(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));
	}

	CollisionShape3D::CollisionShape3D(float innerMargin) :
			innerMargin(innerMargin),
			initialInnerMargin(innerMargin)
	{
		lastTransform.setPosition(Point3<float>(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));
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
		return std::find(CONVEX_SHAPES.begin(), CONVEX_SHAPES.end(), getShapeType()) != CONVEX_SHAPES.end();
	}

	bool CollisionShape3D::isConcave() const
	{
		return std::find(CONCAVE_SHAPES.begin(), CONCAVE_SHAPES.end(), getShapeType()) != CONCAVE_SHAPES.end();
	}

	bool CollisionShape3D::isCompound() const
	{
		return std::find(COMPOUND_SHAPES.begin(), COMPOUND_SHAPES.end(), getShapeType()) != COMPOUND_SHAPES.end();
	}
}
