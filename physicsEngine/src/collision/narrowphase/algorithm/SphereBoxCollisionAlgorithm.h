#ifndef URCHINENGINE_SPHEREBOXCOLLISIONALGORITHM_H
#define URCHINENGINE_SPHEREBOXCOLLISIONALGORITHM_H

#include "collision/narrowphase/algorithm/CollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/CollisionAlgorithmBuilder.h"
#include "collision/ManifoldResult.h"
#include "collision/narrowphase/CollisionObjectWrapper.h"

namespace urchin
{

	class SphereBoxCollisionAlgorithm : public CollisionAlgorithm
	{
		public:
			SphereBoxCollisionAlgorithm(bool, const ManifoldResult &);
			virtual ~SphereBoxCollisionAlgorithm();

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
