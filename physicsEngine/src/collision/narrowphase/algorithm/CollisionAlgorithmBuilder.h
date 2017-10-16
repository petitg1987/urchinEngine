#ifndef URCHINENGINE_COLLISIONALGORITHMBUILDER_H
#define URCHINENGINE_COLLISIONALGORITHMBUILDER_H

#include "collision/narrowphase/algorithm/CollisionAlgorithm.h"
#include "collision/narrowphase/CollisionObjectWrapper.h"
#include "collision/ManifoldResult.h"
#include "shape/CollisionShape3D.h"

namespace urchin
{

	class CollisionAlgorithmBuilder
	{
		public:
			virtual ~CollisionAlgorithmBuilder() = default;

			virtual CollisionAlgorithm *createCollisionAlgorithm(bool, const ManifoldResult &, void *) const = 0;

			virtual std::set<CollisionShape3D::ShapeType> getFirstExpectedShapeType() const = 0;
			virtual unsigned int getAlgorithmSize() const = 0;
	};

}

#endif
