#ifndef URCHINENGINE_COMPOUNDCOLLISIONALGORITHM_H
#define URCHINENGINE_COMPOUNDCOLLISIONALGORITHM_H

#include "collision/narrowphase/algorithm/CollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/CollisionAlgorithmBuilder.h"
#include "collision/narrowphase/algorithm/CollisionAlgorithmSelector.h"
#include "collision/ManifoldResult.h"
#include "collision/narrowphase/CollisionObjectWrapper.h"

namespace urchin
{

	class CompoundCollisionAlgorithm : public CollisionAlgorithm
	{
		public:
			CompoundCollisionAlgorithm(bool, const ManifoldResult &);
			~CompoundCollisionAlgorithm() override;

			void doProcessCollisionAlgorithm(const CollisionObjectWrapper &, const CollisionObjectWrapper &) override;

			struct Builder : public CollisionAlgorithmBuilder
			{
				CollisionAlgorithm *createCollisionAlgorithm(bool, const ManifoldResult &, void*) const override;

				CollisionShape3D::ShapeType getShapeType1() const override;
				CollisionShape3D::ShapeType getShapeType2() const override;
				unsigned int getAlgorithmSize() const override;
			};

		private:
			void addContactPointsToManifold(const ManifoldResult &, bool);

			CollisionAlgorithmSelector *const collisionAlgorithmSelector;
	};

}

#endif
