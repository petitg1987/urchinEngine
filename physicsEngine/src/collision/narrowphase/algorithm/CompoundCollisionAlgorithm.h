#ifndef ENGINE_COMPOUNDCOLLISIONALGORITHM_H
#define ENGINE_COMPOUNDCOLLISIONALGORITHM_H

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
			virtual ~CompoundCollisionAlgorithm();

			void doProcessCollisionAlgorithm(const CollisionObjectWrapper &, const CollisionObjectWrapper &);

			struct Builder : public CollisionAlgorithmBuilder
			{
				CollisionAlgorithm *createCollisionAlgorithm(bool, const ManifoldResult &, void*) const;

				CollisionShape3D::ShapeType getShapeType1() const;
				CollisionShape3D::ShapeType getShapeType2() const;
				unsigned int getAlgorithmSize() const;
			};

		private:
			void addContactPointsToManifold(const ManifoldResult &, bool);

			CollisionAlgorithmSelector *collisionAlgorithmSelector;
	};

}

#endif
