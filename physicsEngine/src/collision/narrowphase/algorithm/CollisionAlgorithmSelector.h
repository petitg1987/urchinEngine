#ifndef URCHINENGINE_COLLISIONALGORITHMSELECTOR_H
#define URCHINENGINE_COLLISIONALGORITHMSELECTOR_H

#include <memory>

#include "body/work/AbstractWorkBody.h"
#include "collision/narrowphase/algorithm/CollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/CollisionAlgorithmBuilder.h"
#include "utils/pool/FixedSizePool.h"

namespace urchin
{

	class CollisionAlgorithmSelector
	{
		public:
			CollisionAlgorithmSelector();
			~CollisionAlgorithmSelector();

			std::shared_ptr<CollisionAlgorithm> createCollisionAlgorithm(AbstractWorkBody *, const CollisionShape3D *, AbstractWorkBody *, const CollisionShape3D *);

		private:
			void initializeCollisionAlgorithmBuilderMatrix();
			void deleteCollisionAlgorithmBuilderMatrix();

			class AlgorithmDeleter
			{
				public:
					AlgorithmDeleter(FixedSizePool<CollisionAlgorithm> *);
					void operator()(CollisionAlgorithm *);

				private:
					FixedSizePool<CollisionAlgorithm> *const algorithmPool;
			};

			FixedSizePool<CollisionAlgorithm> *algorithmPool;
			CollisionAlgorithmBuilder *collisionAlgorithmBuilderMatrix[CollisionShape3D::SHAPE_MAX][CollisionShape3D::SHAPE_MAX];
	};

}

#endif
