#ifndef ENGINE_CONEXHULLCONVEXHULLCOLLISIONALGORITHM_H
#define ENGINE_CONEXHULLCONVEXHULLCOLLISIONALGORITHM_H

#include "collision/narrowphase/algorithm/CollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/CollisionAlgorithmBuilder.h"
#include "collision/ManifoldResult.h"
#include "collision/narrowphase/CollisionObjectWrapper.h"
#include "collision/narrowphase/algorithm/gjk/GJKAlgorithm.h"
#include "collision/narrowphase/algorithm/epa/EPAAlgorithm.h"

namespace urchin
{

	class ConvexHullConvexHullCollisionAlgorithm : public CollisionAlgorithm
	{
		public:
			ConvexHullConvexHullCollisionAlgorithm(bool, const ManifoldResult &);
			virtual ~ConvexHullConvexHullCollisionAlgorithm();

			void doProcessCollisionAlgorithm(const CollisionObjectWrapper &, const CollisionObjectWrapper &);

			struct Builder : public CollisionAlgorithmBuilder
			{
				CollisionAlgorithm *createCollisionAlgorithm(bool, const ManifoldResult &, void*) const;

				CollisionShape3D::ShapeType getShapeType1() const;
				CollisionShape3D::ShapeType getShapeType2() const;
				unsigned int getAlgorithmSize() const;
			};

		private:
			GJKAlgorithm<float> gjkAlgorithm;
			EPAAlgorithm<float> epaAlgorithm;
	};

}

#endif
