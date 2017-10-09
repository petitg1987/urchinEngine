#ifndef URCHINENGINE_CONEXHULLCONVEXHULLCOLLISIONALGORITHM_H
#define URCHINENGINE_CONEXHULLCONVEXHULLCOLLISIONALGORITHM_H

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
			~ConvexHullConvexHullCollisionAlgorithm() override = default;

			void doProcessCollisionAlgorithm(const CollisionObjectWrapper &, const CollisionObjectWrapper &) override;

			struct Builder : public CollisionAlgorithmBuilder
			{
				CollisionAlgorithm *createCollisionAlgorithm(bool, const ManifoldResult &, void*) const override;

				CollisionShape3D::ShapeType getShapeType1() const override;
				CollisionShape3D::ShapeType getShapeType2() const override;
				unsigned int getAlgorithmSize() const override;
			};

		private:
			GJKAlgorithm<double> gjkAlgorithm;
			EPAAlgorithm<double> epaAlgorithm;
	};

}

#endif
