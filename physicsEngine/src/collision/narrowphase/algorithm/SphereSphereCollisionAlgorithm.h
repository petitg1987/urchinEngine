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
			~SphereSphereCollisionAlgorithm() override = default;

			void doProcessCollisionAlgorithm(const CollisionObjectWrapper &, const CollisionObjectWrapper &) override;

			struct Builder : public CollisionAlgorithmBuilder
			{
				CollisionAlgorithm *createCollisionAlgorithm(bool, const ManifoldResult &, void*) const override;

				CollisionShape3D::ShapeType getFirstExpectedShapeType() const override;
				unsigned int getAlgorithmSize() const override;
			};
	};

}

#endif
