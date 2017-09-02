#ifndef URCHINENGINE_SPHERESPHERECOLLISIONALGORITHM_H
#define URCHINENGINE_SPHERESPHERECOLLISIONALGORITHM_H

#include "collision/narrowphase/algorithm/CollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/CollisionAlgorithmBuilder.h"
#include "collision/ManifoldResult.h"
#include "collision/narrowphase/CollisionObjectWrapper.h"

namespace urchin
{

	class SphereSphereCollisionAlgorithm : public CollisionAlgorithm
	{
		public:
			SphereSphereCollisionAlgorithm(bool, const ManifoldResult &);
			virtual ~SphereSphereCollisionAlgorithm();

			void doProcessCollisionAlgorithm(const CollisionObjectWrapper &, const CollisionObjectWrapper &);

			struct Builder : public CollisionAlgorithmBuilder
			{
				CollisionAlgorithm *createCollisionAlgorithm(bool, const ManifoldResult &, void*) const;

				CollisionShape3D::ShapeType getShapeType1() const;
				CollisionShape3D::ShapeType getShapeType2() const;
				unsigned int getAlgorithmSize() const;
			};
	};

}

#endif
