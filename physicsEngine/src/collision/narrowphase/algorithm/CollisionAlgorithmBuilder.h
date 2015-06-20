#ifndef ENGINE_COLLISIONALGORITHMBUILDER_H
#define ENGINE_COLLISIONALGORITHMBUILDER_H

#include "collision/narrowphase/algorithm/CollisionAlgorithm.h"
#include "collision/narrowphase/CollisionObjectWrapper.h"
#include "collision/ManifoldResult.h"
#include "shape/CollisionShape3D.h"

namespace urchin
{

	class CollisionAlgorithmBuilder
	{
		public:
			CollisionAlgorithmBuilder();
			virtual ~CollisionAlgorithmBuilder();

			virtual CollisionAlgorithm *createCollisionAlgorithm(bool, const ManifoldResult &, void *) const = 0;

			virtual CollisionShape3D::ShapeType getShapeType1() const = 0;
			virtual CollisionShape3D::ShapeType getShapeType2() const = 0;
			virtual unsigned int getAlgorithmSize() const = 0;
	};

}

#endif
